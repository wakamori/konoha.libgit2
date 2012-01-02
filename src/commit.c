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

static void kGitCommit_init(CTX ctx, kRawPtr *po)
{
	po->rawptr = NULL;
}

static void kGitCommit_free(CTX ctx, kRawPtr *po)
{
	if (po->rawptr != NULL) {
		git_commit_close((git_commit *)po->rawptr);
		po->rawptr = NULL;
	}
}

DEFAPI(void) defGitCommit(CTX ctx, kclass_t cid, kclassdef_t *cdef)
{
	cdef->name = "GitCommit";
	cdef->init = kGitCommit_init;
	cdef->free = kGitCommit_free;
}

/* ------------------------------------------------------------------------ */

/* Get the author of a commit. */
//## @Native GitSignature GitCommit.author();
KMETHOD GitCommit_author(CTX ctx, ksfp_t *sfp _RIX)
{
	RETURN_(new_ReturnRawPtr(ctx, sfp, (git_signature *)git_commit_author(RawPtr_to(git_commit *, sfp[0]))));
}

/* Close an open commit */
//## @Native void GitCommit.close();
KMETHOD GitCommit_close(CTX ctx, ksfp_t *sfp _RIX)
{
	kGitCommit_free(ctx, sfp[0].p);
	RETURNvoid_();
}

/* Get the committer of a commit. */
//## @Native GitSignature GitCommit.committer();
KMETHOD GitCommit_committer(CTX ctx, ksfp_t *sfp _RIX)
{
	RETURN_(new_ReturnRawPtr(ctx, sfp, (git_signature *)git_commit_committer(RawPtr_to(git_commit *, sfp[0]))));
}

/* Create a new commit in the repository using `git_object` instances as
 * parameters. */
//## @Native @Static GitOid GitCommit.create(GitRepository repo, String update_ref, GitSignature author, GitSignature committer, String message_encoding, String message, GitTree tree, Array<GitCommit> parents);
KMETHOD GitCommit_create(CTX ctx, ksfp_t *sfp _RIX)
{
	git_oid *oid = (git_oid *)KNH_MALLOC(ctx, sizeof(git_oid));
	git_repository *repo = RawPtr_to(git_repository *, sfp[1]);
	const char *update_ref = S_totext(sfp[2].s);
	const git_signature *author = RawPtr_to(const git_signature *, sfp[3]);
	const git_signature *committer = RawPtr_to(const git_signature *, sfp[4]);
	const char *message_encoding = S_totext(sfp[5].s);
	const char *message = S_totext(sfp[6].s);
	const git_tree *tree = RawPtr_to(const git_tree *, sfp[7]);
	kArray *a = sfp[8].a;
	int parent_count = knh_Array_size(a);
	int i;
	const git_commit *parents[parent_count];
	for (i = 0; i < parent_count; i++) {
		parents[i] = (const git_commit *)a->ptrs[i]->rawptr;
	}
	int error = git_commit_create(oid, repo, update_ref, author, committer, message_encoding, message, tree, parent_count, parents);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_commit_create", error);
		KNH_FREE(ctx, oid, sizeof(git_oid));
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, oid));
}

/* Create a new commit in the repository using a variable argument list. */
//## @Native @Static GitOid GitCommit.createV(GitRepository repo, String update_ref, GitSignature author, GitSignature committer, String message_encoding, String message, GitTree tree, ...);
KMETHOD GitCommit_createV(CTX ctx, ksfp_t *sfp _RIX)
{
	git_oid *oid = (git_oid *)KNH_MALLOC(ctx, sizeof(git_oid));
	git_repository *repo = RawPtr_to(git_repository *, sfp[1]);
	const char *update_ref = S_totext(sfp[2].s);
	const git_signature *author = RawPtr_to(const git_signature *, sfp[3]);
	const git_signature *committer = RawPtr_to(const git_signature *, sfp[4]);
	const char *message_encoding = S_totext(sfp[5].s);
	const char *message = S_totext(sfp[6].s);
	const git_tree *tree = RawPtr_to(const git_tree *, sfp[7]);
	size_t parent_count = knh_stack_argc(ctx, sfp);
	const git_commit *parents[parent_count - 8];
	int i;
	for (i = 8; i < parent_count; i++) {
		parents[i - 8] = RawPtr_to(const git_commit *, sfp[i]);
	}
	int error = git_commit_create(oid, repo, update_ref, author, committer, message_encoding, message, tree, parent_count, parents);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_commit_create", error);
		KNH_FREE(ctx, oid, sizeof(git_oid));
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, oid));
}

/* Get the id of a commit. */
//## @Native GitOid GitCommit.id();
KMETHOD GitCommit_id(CTX ctx, ksfp_t *sfp _RIX)
{
	RETURN_(new_ReturnRawPtr(ctx, sfp, (git_oid *)git_commit_id(RawPtr_to(git_commit *, sfp[0]))));
}

/* Lookup a commit object from a repository. */
//## @Native @Static GitCommit GitCommit.lookup(GitRepository repo, GitOid id);
KMETHOD GitCommit_lookup(CTX ctx, ksfp_t *sfp _RIX)
{
	git_commit *commit;
	git_repository *repo = RawPtr_to(git_repository *, sfp[1]);
	const git_oid *id = RawPtr_to(const git_oid *, sfp[2]);
	int error = git_commit_lookup(&commit, repo, id);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_commit_lookup", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, commit));
}

/* Lookup a commit object from a repository, given a prefix of its identifier
 * (short id). */
//## @Native @Static GitCommit GitCommit.lookupPrefix(GitRepository repo, GitOid id, int len);
KMETHOD GitCommit_lookupPrefix(CTX ctx, ksfp_t *sfp _RIX)
{
	git_commit *commit;
	git_repository *repo = RawPtr_to(git_repository *, sfp[1]);
	const git_oid *id = RawPtr_to(const git_oid *, sfp[2]);
	unsigned int len = Int_to(unsigned int, sfp[3]);
	int error = git_commit_lookup_prefix(&commit, repo, id, len);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_commit_lookup_prefix", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, commit));
}

/* Get the full message of a commit. */
//## @Native String GitCommit.message();
KMETHOD GitCommit_message(CTX ctx, ksfp_t *sfp _RIX)
{
	RETURN_(new_String(ctx, git_commit_message(RawPtr_to(git_commit *, sfp[0]))));
}

/* Get the encoding for the message of a commit, as a string representing a
 * standard encoding name. */
//## @Native String GitCommit.messageEncoding();
KMETHOD GitCommit_messageEncoding(CTX ctx, ksfp_t *sfp _RIX)
{
	const char *message_encoding = git_commit_message_encoding(RawPtr_to(git_commit *, sfp[0]));
	RETURN_(new_String(ctx, message_encoding == NULL ? "UTF-8" : message_encoding));
}

/* Get the specified parent of the commit. */
//## @Native GitCommit GitCommit.parent(int n);
KMETHOD GitCommit_parent(CTX ctx, ksfp_t *sfp _RIX)
{
	git_commit *parent;
	git_commit *commit = RawPtr_to(git_commit *, sfp[0]);
	unsigned int n = Int_to(unsigned int, sfp[1]);
	int error = git_commit_parent(&parent, commit, n);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_commit_parent", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, parent));
}

/* Get the oid of a specified parent for a commit. This is different from
 * `git_commit_parent`, which will attempt to load the parent commit from the
 * ODB. */
//## @Native GitOid GitCommit.parentOid(int n);
KMETHOD GitCommit_parentOid(CTX ctx, ksfp_t *sfp _RIX)
{
	git_commit *commit = RawPtr_to(git_commit *, sfp[0]);
	unsigned int n = Int_to(unsigned int, sfp[1]);
	const git_oid *id = git_commit_parent_oid(commit, n);
	if (id == NULL) {
		KNH_NTRACE2(ctx, "git_commit_parent_oid", K_FAILED, KNH_LDATA0);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, (git_oid *)id));
}

/* Get the number of parents of this commit */
//## @Native int GitCommit.parentCount();
KMETHOD GitCommit_parentCount(CTX ctx, ksfp_t *sfp _RIX)
{
	RETURNi_(git_commit_parentcount(RawPtr_to(git_commit *, sfp[0])));
}

/* Get the commit time (i.e. committer time) of a commit. */
//## @Native int GitCommit.time();
KMETHOD GitCommit_time(CTX ctx, ksfp_t *sfp _RIX)
{
	RETURNi_(git_commit_time(RawPtr_to(git_commit *, sfp[0])));
}

/* Get the commit timezone offset (i.e. committer's preferred timezone) of a
 * commit. */
//## @Native int GitCommit.timeOffset();
KMETHOD GitCommit_timeOffset(CTX ctx, ksfp_t *sfp _RIX)
{
	RETURNi_(git_commit_time_offset(RawPtr_to(git_commit *, sfp[0])));
}

/* Get the tree pointed to by a commit. */
//## @Native GitTree GitCommit.tree();
KMETHOD GitCommit_tree(CTX ctx, ksfp_t *sfp _RIX)
{
	git_tree *tree_out;
	git_commit *commit = RawPtr_to(git_commit *, sfp[0]);
	int error = git_commit_tree(&tree_out, commit);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_commit_tree", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, tree_out));
}

/* Get the id of the tree pointed to by a commit. This differs from
 * `git_commit_tree` in that no attempts are made to fetch an object from the
 * ODB. */
//## @Native GitOid GitCommit.treeOid();
KMETHOD GitCommit_treeOid(CTX ctx, ksfp_t *sfp _RIX)
{
	RETURN_(new_ReturnRawPtr(ctx, sfp, (git_oid *)git_commit_tree_oid(RawPtr_to(git_commit *, sfp[0]))));
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
