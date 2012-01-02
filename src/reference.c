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
#include "libgit2.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------ */

static void kGitReference_init(CTX ctx, kRawPtr *po)
{
	po->rawptr = NULL;
}

static void kGitReference_free(CTX ctx, kRawPtr *po)
{
	if (po->rawptr != NULL) {
		git_reference_delete((git_reference *)po->rawptr);
		po->rawptr = NULL;
	}
}

static knh_IntData_t GitReferenceConstInt[] = {
	{"INVALID", GIT_REF_INVALID},
	{"OID", GIT_REF_OID},
	{"SYMBOLIC", GIT_REF_SYMBOLIC},
	{"PACKED", GIT_REF_PACKED},
	{"HAS_PEEL", GIT_REF_HAS_PEEL},
	{"LISTALL", GIT_REF_LISTALL},
	{NULL}
};

DEFAPI(void) constGitReference(CTX ctx, kclass_t cid, const knh_LoaderAPI_t *kapi)
{
	kapi->loadClassIntConst(ctx, cid, GitReferenceConstInt);
}

DEFAPI(void) defGitReference(CTX ctx, kclass_t cid, kclassdef_t *cdef)
{
	cdef->name = "GitReference";
	cdef->init = kGitReference_init;
	cdef->free = kGitReference_free;
}

static int GitReference_callback(const char *name, void *payload)
{
	CTX lctx = knh_getCurrentContext();
	ksfp_t *lsfp = lctx->esp;
	KNH_SETv(lctx, lsfp[K_CALLDELTA + 1].o, new_String(lctx, name));
	kFunc *fo = (kFunc *)payload;
	knh_Func_invoke(lctx, fo, lsfp, 1);
	return lsfp[K_RTNIDX].ivalue;
}

/* ------------------------------------------------------------------------ */

/* Create a new object id reference. */
//## @Native @Static GitReference GitReference.createOid(GitRepository repo, String name, GitOid id, int force);
KMETHOD GitReference_createOid(CTX ctx, ksfp_t *sfp _RIX)
{
	git_reference *ref_out;
	git_repository *repo = RawPtr_to(git_repository *, sfp[1]);
	const char *name = S_totext(sfp[2].s);
	const git_oid *id = RawPtr_to(const git_oid *, sfp[3]);
	int force = Int_to(int, sfp[4]);
	int error = git_reference_create_oid(&ref_out, repo, name, id, force);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_reference_create_oid", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, ref_out));
}

/* Create a new symbolic reference. */
//## @Native @Static GitReference GitReference.createSymbolic(GitRepository repo, String name, String target, int force);
KMETHOD GitReference_createSymbolic(CTX ctx, ksfp_t *sfp _RIX)
{
	git_reference *ref_out;
	git_repository *repo = RawPtr_to(git_repository *, sfp[1]);
	const char *name = S_totext(sfp[2].s);
	const char *target = S_totext(sfp[3].s);
	int force = Int_to(int, sfp[4]);
	int error = git_reference_create_symbolic(&ref_out, repo, name, target, force);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_reference_create_symbolic", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, ref_out));
}

/* Delete an existing reference */
//## @Native void GitReference.delete();
KMETHOD GitReference_delete(CTX ctx, ksfp_t *sfp _RIX)
{
	int error = git_reference_delete(RawPtr_to(git_reference *, sfp[0]));
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_reference_delete", error);
	}
	sfp[0].p->rawptr = NULL;
	RETURNvoid_();
}

/* Perform an operation on each reference in the repository */
//## @Native @Static void GitReference.foreach(GitRepository repo, int list_flags, Func<String=>int> callback);
KMETHOD GitReference_foreach(CTX ctx, ksfp_t *sfp _RIX)
{
	git_repository *repo = RawPtr_to(git_repository *, sfp[1]);
	unsigned int list_flags = Int_to(unsigned int, sfp[2]);
	int (*callback)(const char *, void*) = GitReference_callback;
	void *payload = sfp[3].fo;
	int error = git_reference_foreach(repo, list_flags, callback, payload);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_reference_foreach", error);
	}
	RETURNvoid_();
}

/* Fill a list with all the references that can be found in a repository. */
//## @Native @Static Array<String> GitReference.listAll(GitRepository repo, int list_flags);
KMETHOD GitReference_listAll(CTX ctx, ksfp_t *sfp _RIX)
{
	git_strarray array;
	git_repository *repo = RawPtr_to(git_repository *, sfp[2]);
	unsigned int list_flags = Int_to(unsigned int, sfp[3]);
	int error = git_reference_listall(&array, repo, list_flags);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_reference_listall", error);
		RETURN_(KNH_TNULL(Array));
	}
	kArray *a = new_Array(ctx, CLASS_String, array.count);
	int i;
	for (i = 0; i < array.count; i++) {
		knh_Array_add(ctx, a, array.strings[i]);
	}
	git_strarray_free(&array);
	RETURN_(a);
}

/* Lookup a reference by its name in a repository. */
//## @Native @Static GitReference GitReference.lookup(GitRepository repo, String name);
KMETHOD GitReference_lookup(CTX ctx, ksfp_t *sfp _RIX)
{
	git_reference *reference_out;
	git_repository *repo = RawPtr_to(git_repository *, sfp[1]);
	const char *name = String_to(const char *, sfp[2]);
	int error = git_reference_lookup(&reference_out, repo, name);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_reference_lookup", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, reference_out));
}

/* Get the full name of a reference */
//## @Native String GitReference.name();
KMETHOD GitReference_name(CTX ctx, ksfp_t *sfp _RIX)
{
	git_reference *ref = RawPtr_to(git_reference *, sfp[0]);
	const char *name = git_reference_name(ref);
	RETURN_(new_String(ctx, name));
}

/* Get the OID pointed to by a reference. */
//## @Native GitOid GitReference.oid();
KMETHOD GitReference_oid(CTX ctx, ksfp_t *sfp _RIX)
{
	git_reference *ref = RawPtr_to(git_reference *, sfp[0]);
	const git_oid *id = git_reference_oid(ref);
	if (id == NULL) {
		KNH_NTRACE2(ctx, "git_reference_oid", K_FAILED, KNH_LDATA0);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, (git_oid *)id));
}

/* Get the repository where a reference resides */
//## @Native GitRepository GitReference.owner();
KMETHOD GitReference_owner(CTX ctx, ksfp_t *sfp _RIX)
{
	git_reference *ref = RawPtr_to(git_reference *, sfp[0]);
	git_repository *repo = git_reference_owner(ref);
	RETURN_(new_ReturnRawPtr(ctx, sfp, repo));
}

/* Pack all the loose references in the repository */
//## @Native void GitReference.packAll();
KMETHOD GitReference_packAll(CTX ctx, ksfp_t *sfp _RIX)
{
	git_repository *repo = RawPtr_to(git_repository *, sfp[0]);
	int error = git_reference_packall(repo);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_reference_packall", error);
	}
	RETURNvoid_();
}

/* Rename an existing reference */
//## @Native void GitReference.rename(String new_name, int force);
KMETHOD GitReference_rename(CTX ctx, ksfp_t *sfp _RIX)
{
	git_reference *ref = RawPtr_to(git_reference *, sfp[0]);
	const char *new_name = RawPtr_to(const char *, sfp[1]);
	int force = Int_to(int, sfp[2]);
	int error = git_reference_rename(ref, new_name, force);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_reference_rename", error);
	}
	RETURNvoid_();
}

/* Resolve a symbolic reference */
//## @Native GitReference GitReference.resolve();
KMETHOD GitReference_resolve(CTX ctx, ksfp_t *sfp _RIX)
{
	git_reference *resolved_ref;
	git_reference *ref = RawPtr_to(git_reference *, sfp[0]);
	int error = git_reference_resolve(&resolved_ref, ref);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_reference_resolve", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, resolved_ref));
}

/* Set the OID target of a reference. */
//## @Native void GitReference.setOid(GitOid id);
KMETHOD GitReference_setOid(CTX ctx, ksfp_t *sfp _RIX)
{
	git_reference *ref = RawPtr_to(git_reference *, sfp[0]);
	const git_oid *id = RawPtr_to(const git_oid *, sfp[1]);
	int error = git_reference_set_oid(ref, id);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_reference_set_oid", error);
	}
	RETURNvoid_();
}

/* Set the symbolic target of a reference. */
//## @Native void GitReference.setTarget(String target);
KMETHOD GitReference_setTarget(CTX ctx, ksfp_t *sfp _RIX)
{
	git_reference *ref = RawPtr_to(git_reference *, sfp[0]);
	const char *target = String_to(const char *, sfp[1]);
	int error = git_reference_set_target(ref, target);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_reference_set_target", error);
	}
	RETURNvoid_();
}

/* Get full name to the reference pointed by this reference */
//## @Native String GitReference.target();
KMETHOD GitReference_target(CTX ctx, ksfp_t *sfp _RIX)
{
	git_reference *ref = RawPtr_to(git_reference *, sfp[0]);
	const char *name = git_reference_target(ref);
	RETURN_(new_String(ctx, name));
}

/* Get the type of a reference */
//## @Native int GitReference.type();
KMETHOD GitReference_type(CTX ctx, ksfp_t *sfp _RIX)
{
	git_reference *ref = RawPtr_to(git_reference *, sfp[0]);
	git_rtype type = git_reference_type(ref);
	RETURNi_(type);
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
