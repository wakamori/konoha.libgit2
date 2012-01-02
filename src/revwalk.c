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

static void kGitRevwalk_init(CTX ctx, kRawPtr *po)
{
	po->rawptr = NULL;
}

static void kGitRevwalk_free(CTX ctx, kRawPtr *po)
{
	if (po->rawptr != NULL) {
		git_revwalk_free((git_revwalk *)po->rawptr);
		po->rawptr = NULL;
	}
}

DEFAPI(void) defGitRevwalk(CTX ctx, kclass_t cid, kclassdef_t *cdef)
{
	cdef->name = "GitRevwalk";
	cdef->init = kGitRevwalk_init;
	cdef->free = kGitRevwalk_free;
}

DEFAPI(void) defGitSort(CTX ctx, kclass_t cid, kclassdef_t *cdef)
{
	cdef->name = "GitSort";
}

static knh_IntData_t GitSortConstInt[] = {
	{"NONE", GIT_SORT_NONE},
	{"TOPOLOGICAL", GIT_SORT_TOPOLOGICAL},
	{"TIME", GIT_SORT_TIME},
	{"REVERSE", GIT_SORT_REVERSE},
	{NULL}
};

DEFAPI(void) constGitSort(CTX ctx, kclass_t cid, const knh_LoaderAPI_t *kapi)
{
	kapi->loadClassIntConst(ctx, cid, GitSortConstInt);
}

/* ------------------------------------------------------------------------ */

/* Free a revision walker previously allocated. */
//## @Native void GitRevwalk.free();
KMETHOD GitRevwalk_free(CTX ctx, ksfp_t *sfp _RIX)
{
	kGitRevwalk_free(ctx, sfp[0].p);
	RETURNvoid_();
}

/* Mark a commit (and its ancestors) uninteresting for the output. */
//## @Native void GitRevwalk.hide(GitOid oid);
KMETHOD GitRevwalk_hide(CTX ctx, ksfp_t *sfp _RIX)
{
	git_revwalk *walk = RawPtr_to(git_revwalk *, sfp[0]);
	const git_oid *oid = RawPtr_to(const git_oid *, sfp[1]);
	int error = git_revwalk_hide(walk, oid);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_revwalk_hide", error);
	}
	RETURNvoid_();
}

/* Allocate a new revision walker to iterate through a repo. */
//## @Native GitRevwalk GitRevwalk.new(GitRepository repo);
KMETHOD GitRevwalk_new(CTX ctx, ksfp_t *sfp _RIX)
{
	git_revwalk *walker;
	git_repository *repo = RawPtr_to(git_repository *, sfp[1]);
	int error = git_revwalk_new(&walker, repo);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_revwalk_new", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, walker));
}

/* Get the next commit from the revision walk. */
//## @Native GitOid GitRevwalk.next();
KMETHOD GitRevwalk_next(CTX ctx, ksfp_t *sfp _RIX)
{
	git_oid *oid;
	git_revwalk *walk = RawPtr_to(git_revwalk *, sfp[0]);
	int error = git_revwalk_next(oid, walk);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_revwalk_next", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, oid));
}

/* Mark a commit to start traversal from. */
//## @Native void GitRevwalk.push(GitOid oid);
KMETHOD GitRevwalk_push(CTX ctx, ksfp_t *sfp _RIX)
{
	git_revwalk *walk = RawPtr_to(git_revwalk *, sfp[0]);
	const git_oid *oid = RawPtr_to(const git_oid *, sfp[1]);
	int error = git_revwalk_push(walk, oid);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_revwalk_push", error);
	}
	RETURNvoid_();
}

/* Return the repository on which this walker is operating. */
//## @Native GitRepository GitRevwalk.repository();
KMETHOD GitRevwalk_repository(CTX ctx, ksfp_t *sfp _RIX)
{
	git_revwalk *walk = RawPtr_to(git_revwalk *, sfp[0]);
	RETURN_(new_ReturnRawPtr(ctx, sfp, git_revwalk_repository(walk)));
}

/* Reset the revision walker for reuse. */
//## @Native void GitRevwalk.reset();
KMETHOD GitRevwalk_reset(CTX ctx, ksfp_t *sfp _RIX)
{
	git_revwalk *walker = RawPtr_to(git_revwalk *, sfp[0]);
	git_revwalk_reset(walker);
	RETURNvoid_();
}

/* Change the sorting mode when iterating through the repository's contents. */
//## @Native void GitRevwalk.sorting(int sort_mode);
KMETHOD GitRevwalk_sorting(CTX ctx, ksfp_t *sfp _RIX)
{
	git_revwalk *walk = RawPtr_to(git_revwalk *, sfp[0]);
	unsigned int sort_mode = Int_to(unsigned int, sfp[1]);
	git_revwalk_sorting(walk, sort_mode);
	RETURNvoid_();
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
