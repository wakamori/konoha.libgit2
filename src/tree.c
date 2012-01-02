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

static void kGitTree_init(CTX ctx, kRawPtr *po)
{
	po->rawptr = NULL;
}

static void kGitTree_free(CTX ctx, kRawPtr *po)
{
	if (po->rawptr != NULL) {
		git_tree_close((git_tree *)po->rawptr);
		po->rawptr = NULL;
	}
}

DEFAPI(void) defGitTree(CTX ctx, kclass_t cid, kclassdef_t *cdef)
{
	cdef->name = "GitTree";
	cdef->init = kGitTree_init;
	cdef->free = kGitTree_free;
}

static void kGitTreeEntry_init(CTX ctx, kRawPtr *po)
{
	po->rawptr = NULL;
}

static void kGitTreeEntry_free(CTX ctx, kRawPtr *po)
{
	if (po->rawptr != NULL) {
		// [TODO]
		po->rawptr = NULL;
	}
}

DEFAPI(void) defGitTreeEntry(CTX ctx, kclass_t cid, kclassdef_t *cdef)
{
	cdef->name = "GitTreeEntry";
	cdef->init = kGitTreeEntry_init;
	cdef->free = kGitTreeEntry_free;
}

/* ------------------------------------------------------------------------ */

/* Close an open tree */
//## @Native void GitTree.close();
KMETHOD GitTree_close(CTX ctx, ksfp_t *sfp _RIX)
{
	kGitTree_free(ctx, sfp[0].p);
	RETURNvoid_();
}

/* Write a tree to the ODB from the index file */
//## @Native @Static GitOid GitTree.createFromIndex(GitIndex index);
KMETHOD GitTree_createFromIndex(CTX ctx, ksfp_t *sfp _RIX)
{
	git_oid *oid = (git_oid *)KNH_MALLOC(ctx, sizeof(git_oid));
	git_index *index = RawPtr_to(git_index *, sfp[1]);
	int error = git_tree_create_fromindex(oid, index);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_tree_create_fromindex", error);
		KNH_FREE(ctx, oid, sizeof(git_oid));
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, oid));
}

/* Get the UNIX file attributes of a tree entry */
//## @Native int GitTreeEntry.attributes();
KMETHOD GitTreeEntry_attributes(CTX ctx, ksfp_t *sfp _RIX)
{
	const git_tree_entry *entry = RawPtr_to(const git_tree_entry *, sfp[0]);
	unsigned int i = git_tree_entry_attributes(entry);
	RETURNi_(i);
}

/* Lookup a tree entry by its position in the tree */
//## @Native @Static GitTreeEntry GitTreeEntry.byIndex(GitTree tree, int idx);
KMETHOD GitTreeEntry_byIndex(CTX ctx, ksfp_t *sfp _RIX)
{
	git_tree *tree = RawPtr_to(git_tree *, sfp[1]);
	unsigned int idx = Int_to(unsigned int, sfp[2]);
	const git_tree_entry *entry = git_tree_entry_byindex(tree, idx);
	if (entry == NULL) {
		KNH_NTRACE2(ctx, "git_tree_entry_byindex", K_NOTICE, KNH_LDATA(
					LOG_msg("not found")));
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, (git_tree_entry *)entry));
}

/* Lookup a tree entry by its filename */
//## @Native @Static GitTreeEntry GitTreeEntry.byName(GitTree tree, String filename);
KMETHOD GitTreeEntry_byName(CTX ctx, ksfp_t *sfp _RIX)
{
	git_tree *tree = RawPtr_to(git_tree *, sfp[1]);
	const char *filename = String_to(const char *, sfp[2]);
	const git_tree_entry *entry = git_tree_entry_byname(tree, filename);
	if (entry == NULL) {
		KNH_NTRACE2(ctx, "git_tree_entry_byname", K_NOTICE, KNH_LDATA(
					LOG_msg("not found")));
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, (git_tree_entry *)entry));
}

/* Get the id of the object pointed by the entry */
//## @Native GitOid GitTreeEntry.id();
KMETHOD GitTreeEntry_id(CTX ctx, ksfp_t *sfp _RIX)
{
	const git_tree_entry *entry = RawPtr_to(const git_tree_entry *, sfp[0]);
	const git_oid *id = git_tree_entry_id(entry);
	RETURN_(new_ReturnRawPtr(ctx, sfp, (git_oid *)id));
}

/* Get the filename of a tree entry */
//## @Native String GitTreeEntry.name();
KMETHOD GitTreeEntry_name(CTX ctx, ksfp_t *sfp _RIX)
{
	const git_tree_entry *entry = RawPtr_to(const git_tree_entry *, sfp[0]);
	RETURN_(new_String(ctx, git_tree_entry_name(entry)));
}

/* Get the type of the object pointed by the entry */
//## @Native int GitTreeEntry.type();
KMETHOD GitTreeEntry_type(CTX ctx, ksfp_t *sfp _RIX)
{
	const git_tree_entry *entry = RawPtr_to(const git_tree_entry *, sfp[0]);
	RETURNi_(git_tree_entry_type(entry));
}

/* Get the number of entries listed in a tree */
//## @Native int GitTree.entryCount();
KMETHOD GitTree_entryCount(CTX ctx, ksfp_t *sfp _RIX)
{
	git_tree *tree = RawPtr_to(git_tree *, sfp[0]);
	RETURNi_(git_tree_entrycount(tree));
}

///* Retrieve the tree object containing a tree entry, given a relative path to
// * this tree entry */
////## @Native @Static GitTree GitTree.fromPath(GitTree root, Path treeentry_path);
//KMETHOD GitTree_fromPath(CTX ctx, ksfp_t *sfp _RIX)
//{
//	git_tree *parent_out;
//	git_tree *tree = RawPtr_to(git_tree *, sfp[1]);
//	const char *treeentry_path = String_to(const char *, sfp[2]);
//	int error = git_tree_frompath(&parent_out, tree, treeentry_path);
//	if (error < GIT_SUCCESS) {
//		TRACE_ERROR(ctx, "git_tree_frompath", error);
//		RETURN_(KNH_NULL);
//	}
//	RETURN_(new_ReturnRawPtr(ctx, sfp, parent_out));
//}

/* Get the id of a tree. */
//## @Native GitOid GitTree.id();
KMETHOD GitTree_id(CTX ctx, ksfp_t *sfp _RIX)
{
	git_tree *tree = RawPtr_to(git_tree *, sfp[0]);
	const git_oid *id = git_tree_id(tree);
	RETURN_(new_ReturnRawPtr(ctx, sfp, (git_oid *)id));
}

/* Lookup a tree object from the repository. */
//## @Native @Static GitTree GitTree.lookup(GitRepository repo, GitOid id);
KMETHOD GitTree_lookup(CTX ctx, ksfp_t *sfp _RIX)
{
	git_tree *tree;
	git_repository *repo = RawPtr_to(git_repository *, sfp[1]);
	const git_oid *id = RawPtr_to(const git_oid *, sfp[2]);
	int error = git_tree_lookup(&tree, repo, id);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_tree_lookup", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, tree));
}

/* Lookup a tree object from the repository, given a prefix of its identifier
 * (short id). */
//## @Native @Static GitTree GitTree.lookupPrefix(GitRepository repo, GitOid id, int len);
KMETHOD GitTree_lookupPrefix(CTX ctx, ksfp_t *sfp _RIX)
{
	git_tree *tree;
	git_repository *repo = RawPtr_to(git_repository *, sfp[1]);
	const git_oid *id = RawPtr_to(const git_oid *, sfp[2]);
	int len = Int_to(int, sfp[3]);
	int error = git_tree_lookup_prefix(&tree, repo, id, len);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_tree_lookup_prefix", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, tree));
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
