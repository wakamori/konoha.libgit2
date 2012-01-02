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

#define K_INTERNAL
#include <konoha1.h>
#include <konoha1/inlinelibs.h>
#include "libgit2.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------ */

static void kGitTreebuilder_init(CTX ctx, kRawPtr *po)
{
	po->rawptr = NULL;
}

static void kGitTreebuilder_free(CTX ctx, kRawPtr *po)
{
	if (po->rawptr != NULL) {
		git_treebuilder_free((git_treebuilder *)po->rawptr);
		po->rawptr = NULL;
	}
}

DEFAPI(void) defGitTreebuilder(CTX ctx, kclass_t cid, kclassdef_t *cdef)
{
	cdef->name = "GitTreebuilder";
	cdef->init = kGitTreebuilder_init;
	cdef->free = kGitTreebuilder_free;
}

static int kGitTreebuilder_filter(const git_tree_entry *entry, void *payload)
{
	CTX lctx = knh_getCurrentContext();
	ksfp_t *lsfp = lctx->esp;
	KNH_SETv(lctx, lsfp[K_CALLDELTA + 1].o, new_RawPtr(lctx, lctx->share->ClassTBL[knh_getcid(lctx, STEXT("GitTreeEntry"))], (git_tree_entry *)entry));
	kFunc *fo = payload;
	knh_Func_invoke(lctx, fo, lsfp, 1);
	return lsfp[K_RTNIDX].ivalue;
}

/* ------------------------------------------------------------------------ */

/* Clear all the entires in the builder */
//## @Native void GitTreebuilder.clear();
KMETHOD GitTreebuilder_clear(CTX ctx, ksfp_t *sfp _RIX)
{
	git_treebuilder *bld = RawPtr_to(git_treebuilder *, sfp[0]);
	git_treebuilder_clear(bld);
	RETURNvoid_();
}

/* Create a new tree builder. */
//## @Native @Static GitTreebuilder GitTreebuilder.create(GitTree source);
KMETHOD GitTreebuilder_create(CTX ctx, ksfp_t *sfp _RIX)
{
	git_treebuilder *builder_p;
	const git_tree *source = RawPtr_to(const git_tree *, sfp[1]);
	int error = git_treebuilder_create(&builder_p, source);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_treebuilder_create", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, builder_p));
}

/* Filter the entries in the tree */
//## @Native void GitTreebuilder.filter(Func<GitTreeEntry=>int> filter);
KMETHOD GitTreebuilder_filter(CTX ctx, ksfp_t *sfp _RIX)
{
	git_treebuilder *bld = RawPtr_to(git_treebuilder *, sfp[0]);
	int (*filter)(const git_tree_entry *, void *) = kGitTreebuilder_filter;
	void *payload = sfp[1].fo;
	git_treebuilder_filter(bld, filter, payload);
	RETURNvoid_();
}

/* Free a tree builder */
//## @Native void GitTreebuilder.free();
KMETHOD GitTreebuilder_free(CTX ctx, ksfp_t *sfp _RIX)
{
	kGitTreebuilder_free(ctx, sfp[0].p);
	RETURNvoid_();
}

/* Get an entry from the builder from its filename */
//## @Native GitTreeEntry GitTreebuilder.get(String filename);
KMETHOD GitTreebuilder_get(CTX ctx, ksfp_t *sfp _RIX)
{
	git_treebuilder *bld = RawPtr_to(git_treebuilder *, sfp[0]);
	const char *filename = String_to(const char *, sfp[1]);
	const git_tree_entry *entry = git_treebuilder_get(bld, filename);
	if (entry == NULL) {
		KNH_NTRACE2(ctx, "git_treebuilder_get", K_NOTICE, KNH_LDATA(
					LOG_msg("not found")));
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, bld));
}

/* Add or update an entry to the builder */
//## @Native GitTreeEntry GitTreebuilder.insert(String filename, GitOid id, int attributes);
KMETHOD GitTreebuilder_insert(CTX ctx, ksfp_t *sfp _RIX)
{
	git_tree_entry *entry_out;
	git_treebuilder *bld = RawPtr_to(git_treebuilder *, sfp[0]);
	const char *filename = String_to(const char *, sfp[1]);
	const git_oid *id = RawPtr_to(const git_oid *, sfp[2]);
	int attributes = Int_to(int, sfp[3]);
	int error = git_treebuilder_insert(&entry_out, bld, filename, id, attributes);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_treebuilder_insert", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, entry_out));
}

/* Remove an entry from the builder by its filename */
//## @Native void GitTreebuilder.remove(String filename);
KMETHOD GitTreebuilder_remove(CTX ctx, ksfp_t *sfp _RIX)
{
	git_treebuilder *bld = RawPtr_to(git_treebuilder *, sfp[0]);
	const char *filename = String_to(const char *, sfp[1]);
	int error = git_treebuilder_remove(bld, filename);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_treebuilder_remove", error);
	}
	RETURNvoid_();
}

/* Write the contents of the tree builder as a tree object */
//## @Native GitOid GitTreebuilder.write(GitRepository repo);
KMETHOD GitTreebuilder_write(CTX ctx, ksfp_t *sfp _RIX)
{
	git_oid *oid = (git_oid *)KNH_MALLOC(ctx, sizeof(git_oid));
	git_treebuilder *bld = RawPtr_to(git_treebuilder *, sfp[0]);
	git_repository *repo = RawPtr_to(git_repository *, sfp[1]);
	int error = git_treebuilder_write(oid, repo, bld);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_treebuilder_write", error);
		KNH_FREE(ctx, oid, sizeof(git_oid));
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, oid));
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
