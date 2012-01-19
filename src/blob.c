/****************************************************************************
 * KONOHA COPYRIGHT, LICENSE NOTICE, AND DISCRIMER
 *
 * Copyright (c)  2010-      Konoha Team konohaken@googlegroups.com
 * All rights reserved.
 *
 * You may choose one of the following two licenses when you use konoha.
 * See www.konohaware.org/license.html for further information.
 *
 * (1) GNU Lesser General Public License 3.0 (with KONOHA_UNDER_LGPL3)
 * (2) Konoha Software Foundation License 1.0
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

// **************************************************************************
// LIST OF CONTRIBUTERS
//  chen_ji - Takuma Wakamori, Yokohama National University, Japan
// **************************************************************************

#include <konoha1.h>
#include "libgit2.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------ */

static void kGitBlob_init(CTX ctx, kRawPtr *po)
{
	po->rawptr = NULL;
}

static void kGitBlob_free(CTX ctx, kRawPtr *po)
{
	if (po->rawptr != NULL) {
		git_blob_close((git_blob *)po->rawptr);
		po->rawptr = NULL;
	}
}

DEFAPI(void) defGitBlob(CTX ctx, kclass_t cid, kclassdef_t *cdef)
{
	cdef->name = "GitBlob";
	cdef->init = kGitBlob_init;
	cdef->free = kGitBlob_free;
}

/* ------------------------------------------------------------------------ */

/* Close an open blob */
//## @Native void GitBlob.close();
KMETHOD GitBlob_close(CTX ctx, ksfp_t *sfp _RIX)
{
	kGitBlob_free(ctx, sfp[0].p);
	RETURNvoid_();
}

/* Write an in-memory buffer to the ODB as a blob */
//## @Native @Static GitOid GitBlob.createFromBuffer(GitRepository repo, Bytes buffer);
KMETHOD GitBlob_createFromBuffer(CTX ctx, ksfp_t *sfp _RIX)
{
	git_oid *oid = (git_oid *)KNH_MALLOC(ctx, sizeof(git_oid));
	git_repository *repo = RawPtr_to(git_repository *, sfp[1]);
	const void *buffer = (const void *)BA_totext(sfp[2].ba);
	size_t len = BA_size(sfp[2].ba);
	int error = git_blob_create_frombuffer(oid, repo, buffer, len);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_blob_create_frombuffer", error);
		KNH_FREE(ctx, oid, sizeof(git_oid));
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, oid));
}

/* Read a file from the working folder of a repository and write it to the
 * Object Database as a loose blob */
//## @Native @Static GitOid GitBlob.createFromFile(GitRepository repo, String path);
KMETHOD GitBlob_createFromFile(CTX ctx, ksfp_t *sfp _RIX)
{
	git_oid *oid = (git_oid *)KNH_MALLOC(ctx, sizeof(git_oid));
	git_repository *repo = RawPtr_to(git_repository *, sfp[1]);
	const char *path = S_totext(sfp[2].s);
	int error = git_blob_create_fromfile(oid, repo, path);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_blob_create_fromfile", error);
		KNH_FREE(ctx, oid, sizeof(git_oid));
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, oid));
}

/* Lookup a blob object from a repository */
//## @Native @Static GitBlob GitBlob.lookup(GitRepository repo, GitOid id);
KMETHOD GitBlob_lookup(CTX ctx, ksfp_t *sfp _RIX)
{
	git_blob *blob;
	git_repository *repo = RawPtr_to(git_repository *, sfp[1]);
	const git_oid *id = RawPtr_to(const git_oid *, sfp[2]);
	int error = git_blob_lookup(&blob, repo, id);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_blob_lookup", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, blob));
}

/* Lookup a blob object from a repository, given a prefix of its identifier
 * (short id). */
//## @Native @Static GitBlob GitBlob.lookupPrefix(GitRepository repo, GitOid id, int len);
KMETHOD GitBlob_lookupPrefix(CTX ctx, ksfp_t *sfp _RIX)
{
	git_blob *blob;
	git_repository *repo = RawPtr_to(git_repository *, sfp[1]);
	const git_oid *id = RawPtr_to(const git_oid *, sfp[2]);
	unsigned int len = Int_to(unsigned int, sfp[3]);
	int error = git_blob_lookup_prefix(&blob, repo, id, len);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_blob_lookup_prefix", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, blob));
}

/* Get a read-only buffer with the raw content of a blob. */
//## @Native Bytes GitBlob.rawContent();
KMETHOD GitBlob_rawContent(CTX ctx, ksfp_t *sfp _RIX)
{
	git_blob *blob = RawPtr_to(git_blob *, sfp[0]);
	const void *rawcontent = git_blob_rawcontent(blob);
	if (rawcontent == NULL) {
		// no content
		KNH_NTRACE2(ctx, "git_blob_rawcontent", K_NOTICE,
				KNH_LDATA(LOG_msg("the blob has no contents")));
		RETURN_(KNH_NULL);
	}
	size_t rawsize = git_blob_rawsize(blob);
	kBytes *ba = new_Bytes(ctx, "git_blob_rawcontent", rawsize);
	knh_Bytes_write2(ctx, ba, rawcontent, rawsize);
	RETURN_(ba);
}

/* Get the size in bytes of the contents of a blob */
//## @Native int GitBlob.rawSize();
KMETHOD GitBlob_rawSize(CTX ctx, ksfp_t *sfp _RIX)
{
	git_blob *blob = RawPtr_to(git_blob *, sfp[0]);
	if (blob == NULL) {
		RETURNi_(0);
	}
	RETURNi_(git_blob_rawsize(blob));
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
