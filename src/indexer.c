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

static void kGitIndexer_init(CTX ctx, kRawPtr *po)
{
	po->rawptr = NULL;
}

static void kGitIndexer_free(CTX ctx, kRawPtr *po)
{
	if (po->rawptr != NULL) {
		git_indexer_free((git_indexer *)po->rawptr);
		po->rawptr = NULL;
	}
}

DEFAPI(void) defGitIndexer(CTX ctx, kclass_t cid, kclassdef_t *cdef)
{
	cdef->name = "GitIndexer";
	cdef->init = kGitIndexer_init;
	cdef->free = kGitIndexer_free;
}

static void kGitIndexerStats_init(CTX ctx, kRawPtr *po)
{
	po->rawptr = NULL;
}

static void kGitIndexerStats_free(CTX ctx, kRawPtr *po)
{
	if (po->rawptr != NULL) {
		KNH_FREE(ctx, po->rawptr, sizeof(git_indexer_stats));
		po->rawptr = NULL;
	}
}

DEFAPI(void) defGitIndexerStats(CTX ctx, kclass_t cid, kclassdef_t *cdef)
{
	cdef->name = "GitIndexerStats";
	cdef->init = kGitIndexerStats_init;
	cdef->free = kGitIndexerStats_free;
}

/* ------------------------------------------------------------------------ */

/* Free the indexer and its resources */
//## @Native void GitIndexer.free();
KMETHOD GitIndexer_free(CTX ctx, ksfp_t *sfp _RIX)
{
	kGitIndexer_free(ctx, sfp[0].p);
	RETURNvoid_();
}

/* Get the packfile's hash */
//## @Native GitOid GitIndexer.hash();
KMETHOD GitIndexer_hash(CTX ctx, ksfp_t *sfp _RIX)
{
	git_indexer *idx = RawPtr_to(git_indexer *, sfp[0]);
	const git_oid *id = git_indexer_hash(idx);
	if (id == NULL) {
		KNH_NTRACE2(ctx, "git_indexer_hash", K_FAILED, KNH_LDATA0);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, (git_oid *)id));
}

/* Create a new indexer instance */
//## @Native GitIndexer GitIndexer.new(String packname);
KMETHOD GitIndexer_new(CTX ctx, ksfp_t *sfp _RIX)
{
	git_indexer *out = RawPtr_to(git_indexer *, sfp[0]);
	const char *packname = S_totext(sfp[1].s);
	int error = git_indexer_new(&out, packname);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_indexer_new", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, out));
}

/* Iterate over the objects in the packfile and extract the information */
//## @Native GitIndexerStats GitIndexer.run();
KMETHOD GitIndexer_run(CTX ctx, ksfp_t *sfp _RIX)
{
	git_indexer *idx = RawPtr_to(git_indexer *, sfp[0]);
	git_indexer_stats *stats = (git_indexer_stats *)KNH_MALLOC(ctx, sizeof(git_indexer_stats));
	int error = git_indexer_run(idx, stats);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_indexer_run", error);
		KNH_FREE(ctx, stats, sizeof(git_indexer_stats));
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, stats));
}

/* Write the index file to disk. */
//## @Native void GitIndexer.write();
KMETHOD GitIndexer_write(CTX ctx, ksfp_t *sfp _RIX)
{
	git_indexer *idx = RawPtr_to(git_indexer *, sfp[0]);
	int error = git_indexer_write(idx);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_indexer_write", error);
	}
	RETURNvoid_();
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
