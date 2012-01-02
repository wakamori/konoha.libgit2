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

static void kGitObject_init(CTX ctx, kRawPtr *po)
{
	po->rawptr = NULL;
}

static void kGitObject_free(CTX ctx, kRawPtr *po)
{
	if (po->rawptr != NULL) {
		git_object_close((git_object *)po->rawptr);
		po->rawptr = NULL;
	}
}

DEFAPI(void) defGitObject(CTX ctx, kclass_t cid, kclassdef_t *cdef)
{
	cdef->name = "GitObject";
	cdef->init = kGitObject_init;
	cdef->free = kGitObject_free;
}

static knh_IntData_t GitObjectConstInt[] = {
	{"ANY", GIT_OBJ_ANY},
	{"BAD", GIT_OBJ_BAD},
	{"_EXT1", GIT_OBJ__EXT1},
	{"COMMIT", GIT_OBJ_COMMIT},
	{"TREE", GIT_OBJ_TREE},
	{"BLOB", GIT_OBJ_BLOB},
	{"TAG", GIT_OBJ_TAG},
	{"_EXT2", GIT_OBJ__EXT2},
	{"OFS_DELTA", GIT_OBJ_OFS_DELTA},
	{"REF_DELTA", GIT_OBJ_REF_DELTA},
	{NULL}
};

DEFAPI(void) constGitObject(CTX ctx, kclass_t cid, const knh_LoaderAPI_t *kapi)
{
	kapi->loadClassIntConst(ctx, cid, GitObjectConstInt);
}

/* ------------------------------------------------------------------------ */

/* Get the size in bytes for the structure which acts as an in-memory
 * representation of any given object type. */
//## @Native @Static int GitObject.size(int type);
KMETHOD GitObject_size(CTX ctx, ksfp_t *sfp _RIX)
{
	RETURNi_(git_object__size(Int_to(git_otype, sfp[1])));
}

/* Close an open object */
//## @Native void GitObject.close();
KMETHOD GitObject_close(CTX ctx, ksfp_t *sfp _RIX)
{
	kGitObject_free(ctx, sfp[0].p);
	RETURNvoid_();
}

/* Get the id (SHA1) of a repository object */
//## @Native GitOid GitObject.id();
KMETHOD GitObject_id(CTX ctx, ksfp_t *sfp _RIX)
{
	const git_oid *id = git_object_id(RawPtr_to(const git_object *, sfp[0]));
	RETURN_(new_ReturnRawPtr(ctx, sfp, (git_oid *)id));
}

/* Lookup a reference to one of the objects in a repostory. */
//## @Native @Static GitObject GitObject.lookup(GitRepository repo, GitOid id, int type);
KMETHOD GitObject_lookup(CTX ctx, ksfp_t *sfp _RIX)
{
	git_object *object;
	git_repository *repo = RawPtr_to(git_repository *, sfp[1]);
	const git_oid *id = RawPtr_to(const git_oid *, sfp[2]);
	git_otype type = Int_to(git_otype, sfp[3]);
	int error = git_object_lookup(&object, repo, id, type);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_object_lookup", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, object));
}

/* Lookup a reference to one of the objects in a repostory, given a prefix of
 * its identifier (short id). */
//## @Native @Static GitObject GitObject.lookupPrefix(GitRepository repo, GitOid id, int len, int type);
KMETHOD GitObject_lookupPrefix(CTX ctx, ksfp_t *sfp _RIX)
{
	git_object *object_out;
	git_repository *repo = RawPtr_to(git_repository *, sfp[1]);
	const git_oid *id = RawPtr_to(const git_oid *, sfp[2]);
	unsigned int len = Int_to(unsigned int, sfp[3]);
	git_otype type = Int_to(git_otype, sfp[4]);
	int error = git_object_lookup_prefix(&object_out, repo, id, len, type);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_object_lookup_prefix", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, object_out));
}

/* Get the repository that owns this object */
//## @Native GitRepository GitObject.owner();
KMETHOD GitObject_owner(CTX ctx, ksfp_t *sfp _RIX)
{
	const git_object *obj = RawPtr_to(const git_object *, sfp[0]);
	git_repository *repo = git_object_owner(obj);
	RETURN_(new_ReturnRawPtr(ctx, sfp, repo));
}

/* Get the object type of an object */
//## @Native int GitObject.type();
KMETHOD GitObject_type(CTX ctx, ksfp_t *sfp _RIX)
{
	const git_object *obj = RawPtr_to(const git_object *, sfp[0]);
	git_otype type = git_object_type(obj);
	RETURNi_(type);
}

/* Determine if the given git_otype is a valid loose object type. */
//## @Native @Static boolean GitObject.typeisLoose(int type);
KMETHOD GitObject_typeisLoose(CTX ctx, ksfp_t *sfp _RIX)
{
	RETURNb_(git_object_typeisloose(Int_to(git_otype, sfp[1])) == 0 ? 0 : 1);
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
