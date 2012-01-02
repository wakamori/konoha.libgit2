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

static void kGitOid_init(CTX ctx, kRawPtr *po)
{
	po->rawptr = NULL;
}

static void kGitOid_free(CTX ctx, kRawPtr *po)
{
	if (po->rawptr != NULL) {
		KNH_FREE(ctx, po->rawptr, sizeof(git_oid));
		po->rawptr = NULL;
	}
}

/* Compare two oid structures. */
static int kGitOid_compareTo(kRawPtr *p1, kRawPtr *p2)
{
	const git_oid *a = (const git_oid *)p1->rawptr;
	const git_oid *b = (const git_oid *)p2->rawptr;
	return git_oid_cmp(a, b);
}

DEFAPI(void) defGitOid(CTX ctx, kclass_t cid, kclassdef_t *cdef)
{
	cdef->name = "GitOid";
	cdef->init = kGitOid_init;
	cdef->free = kGitOid_free;
	cdef->compareTo = kGitOid_compareTo;
}

static void kGitOidShorten_init(CTX ctx, kRawPtr *po)
{
	po->rawptr = NULL;
}

static void kGitOidShorten_free(CTX ctx, kRawPtr *po)
{
	if (po->rawptr != NULL) {
		git_oid_shorten_free((git_oid_shorten *)po->rawptr);
		po->rawptr = NULL;
	}
}

DEFAPI(void) defGitOidShorten(CTX ctx, kclass_t cid, kclassdef_t *cdef)
{
	cdef->name = "GitOidShorten";
	cdef->init = kGitOidShorten_init;
	cdef->free = kGitOidShorten_free;
}

/* ------------------------------------------------------------------------ */

/* Format a git_oid into a newly allocated c-string. */
//## @Native String GitOid.allocfmt();
KMETHOD GitOid_allocfmt(CTX ctx, ksfp_t *sfp _RIX)
{
	const git_oid *oid = RawPtr_to(const git_oid *, sfp[0]);
	char *s = git_oid_allocfmt(oid);
	kString *str = new_String(ctx, s);
	free(s);
	RETURN_(str);
}

/* Copy an oid from one structure to another. */
//## @Native GitOid GitOid.cpy();
KMETHOD GitOid_cpy(CTX ctx, ksfp_t *sfp _RIX)
{
	git_oid *out = (git_oid *)KNH_MALLOC(ctx, sizeof(git_oid));
	const git_oid *src = RawPtr_to(const git_oid *, sfp[0]);
	git_oid_cpy(out, src);
	RETURN_(new_ReturnRawPtr(ctx, sfp, out));
}

/* Format a git_oid into a hex string. */
//## @Native String GitOid.fmt();
KMETHOD GitOid_fmt(CTX ctx, ksfp_t *sfp _RIX)
{
	char str[GIT_OID_HEXSZ + 1] = {0};
	const git_oid *oid = RawPtr_to(const git_oid *, sfp[0]);
	git_oid_fmt(str, oid);
	RETURN_(new_String(ctx, str));
}

/* Copy an already raw oid into a git_oid structure. */
//## @Native @Static GitOid GitOid.fromRaw(Bytes raw);
KMETHOD GitOid_fromRaw(CTX ctx, ksfp_t *sfp _RIX)
{
	git_oid *out = (git_oid *)KNH_MALLOC(ctx, sizeof(git_oid));
	const unsigned char *raw = BA_tobytes(sfp[1].ba).utext;
	git_oid_fromraw(out, raw);
	RETURN_(new_ReturnRawPtr(ctx, sfp, out));
}

/* Parse a hex formatted object id into a git_oid. */
//## @Native @Static GitOid GitOid.fromStr(String str);
KMETHOD GitOid_fromStr(CTX ctx, ksfp_t *sfp _RIX)
{
	git_oid *out = (git_oid *)KNH_MALLOC(ctx, sizeof(git_oid));
	const char *str = String_to(const char *, sfp[1]);
	git_oid_fromstr(out, str);
	RETURN_(new_ReturnRawPtr(ctx, sfp, out));
}

/* Parse N characters of a hex formatted object id into a git_oid */
//## @Native @Static GitOid GitOid.fromStrn(String str, int length);
KMETHOD GitOid_fromStrn(CTX ctx, ksfp_t *sfp _RIX)
{
	git_oid *out = (git_oid *)KNH_MALLOC(ctx, sizeof(git_oid));
	const char *str = String_to(const char *, sfp[1]);
	size_t length = Int_to(size_t, sfp[2]);
	int error = git_oid_fromstrn(out, str, length);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_oid_fromstrn", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, out));
}

/* Compare the first 'len' hexadecimal characters (packets of 4 bits) of two oid
 * structures. */
//## @Native int GitOid.ncmp(GitOid a, GitOid b, int len);
KMETHOD GitOid_ncmp(CTX ctx, ksfp_t *sfp _RIX)
{
	const git_oid *a = RawPtr_to(const git_oid *, sfp[0]);
	const git_oid *b = RawPtr_to(const git_oid *, sfp[1]);
	unsigned int len = Int_to(unsigned int, sfp[2]);
	RETURNi_(git_oid_ncmp(a, b, len));
}

/* Format a git_oid into a loose-object path string. */
//## @Native String GitOid.pathfmt();
KMETHOD GitOid_pathfmt(CTX ctx, ksfp_t *sfp _RIX)
{
	char str[GIT_OID_HEXSZ + 2] = {0};
	const git_oid *oid = RawPtr_to(const git_oid *, sfp[0]);
	git_oid_pathfmt(str, oid);
	RETURN_(new_String(ctx, str));
}

/* Add a new OID to set of shortened OIDs and calculate the minimal length to
 * uniquely identify all the OIDs in the set. */
//## @Native void GitOidShorten.add(String text_oid);
KMETHOD GitOidShorten_add(CTX ctx, ksfp_t *sfp _RIX)
{
	git_oid_shorten *os = RawPtr_to(git_oid_shorten *, sfp[0]);
	const char *text_oid = String_to(const char *, sfp[1]);
	int error = git_oid_shorten_add(os, text_oid);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_oid_shorten_add", error);
	}
	RETURNvoid_();
}

/* Free an OID shortener instance */
//## @Native void GitOidShorten.free();
KMETHOD GitOidShorten_free(CTX ctx, ksfp_t *sfp _RIX)
{
	kGitOidShorten_free(ctx, sfp[0].p);
	RETURNvoid_();
}

///* Check if an oid equals an hex formatted object id. */
////## @Native int GitOid.streq(String str);
//KMETHOD GitOid_streq(CTX ctx, ksfp_t *sfp _RIX)
//{
//	const git_oid *a = RawPtr_to(const git_oid *, sfp[0]);
//	const char *str = String_to(const char *, sfp[1]);
//	RETURNi_(git_oid_streq(a, str));
//}

/* Format a git_oid into a buffer as a hex format c-string. */
//## @Native String GitOid.toString(int n);
KMETHOD GitOid_toString(CTX ctx, ksfp_t *sfp _RIX)
{
	const git_oid *oid = RawPtr_to(const git_oid *, sfp[0]);
	size_t n = Int_to(size_t, sfp[1]);
	n = n == 0 ? GIT_OID_HEXSZ + 1 : n;
	char out[GIT_OID_HEXSZ + 2] = {0};
	char *str = git_oid_to_string(out, n, oid);
	RETURN_(new_String(ctx, str));
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
