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

static void kGitTag_init(CTX ctx, kRawPtr *po)
{
	po->rawptr = NULL;
}

static void kGitTag_free(CTX ctx, kRawPtr *po)
{
	if (po->rawptr != NULL) {
		git_tag_close((git_tag *)po->rawptr);
		po->rawptr = NULL;
	}
}

DEFAPI(void) defGitTag(CTX ctx, kclass_t cid, kclassdef_t *cdef)
{
	cdef->name = "GitTag";
	cdef->init = kGitTag_init;
	cdef->free = kGitTag_free;
}

/* ------------------------------------------------------------------------ */

/* Close an open tag */
//## @Native void GitTag.close();
KMETHOD GitTag_close(CTX ctx, ksfp_t *sfp _RIX)
{
	kGitTag_free(ctx, sfp[0].p);
	RETURNvoid_();
}

/* Create a new tag in the repository from an object */
//## @Native @Static GitOid GitTag.create(GitRepository repo, String tag_name, GitObject target, GitSignature tagger, String message, int force);
KMETHOD GitTag_create(CTX ctx, ksfp_t *sfp _RIX)
{
	git_oid *oid;
	git_repository *repo = RawPtr_to(git_repository *, sfp[1]);
	const char *tag_name = String_to(const char *, sfp[2]);
	const git_object *target = RawPtr_to(const git_object *, sfp[3]);
	const git_signature *tagger = RawPtr_to(const git_signature *, sfp[4]);
	const char *message = String_to(const char *, sfp[5]);
	int force = Int_to(int, sfp[6]);
	int error = git_tag_create(oid, repo, tag_name, target, tagger, message, force);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_tag_create", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, oid));
}

/* Create a new tag in the repository from a buffer */
//## @Native @Static GitOid GitTag.createFromBuffer(GitRepository repo, Bytes buffer, int force);
KMETHOD GitTag_createFromBuffer(CTX ctx, ksfp_t *sfp _RIX)
{
	git_oid *oid;
	git_repository *repo = RawPtr_to(git_repository *, sfp[1]);
	const char *buffer = String_to(const char *, sfp[2]);
	int force = Int_to(int, sfp[3]);
	int error = git_tag_create_frombuffer(oid, repo, buffer, force);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_tag_create_frombuffer", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, oid));
}

/* Create a new lightweight tag pointing at a target object */
//## @Native GitOid GitTag.createLightweight(GitRepository repo, String tag_name, GitObject target, int force);
KMETHOD GitTag_createLightweight(CTX ctx, ksfp_t *sfp _RIX)
{
	git_oid *oid;
	git_repository *repo = RawPtr_to(git_repository *, sfp[1]);
	const char *tag_name = String_to(const char *, sfp[2]);
	const git_object *target = RawPtr_to(const git_object *, sfp[3]);
	int force = Int_to(int, sfp[4]);
	int error = git_tag_create_lightweight(oid, repo, tag_name, target, force);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_tag_create_lightweight", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, oid));
}

/* Delete an existing tag reference. */
//## @Native @Static void GitTag.delete(GitRepository repo, String tag_name);
KMETHOD GitTag_delete(CTX ctx, ksfp_t *sfp _RIX)
{
	git_repository *repo = RawPtr_to(git_repository *, sfp[1]);
	const char *tag_name = String_to(const char *, sfp[2]);
	int error = git_tag_delete(repo, tag_name);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_tag_delete", error);
	}
	RETURNvoid_();
}

/* Get the id of a tag. */
//## @Native GitOid GitTag.id();
KMETHOD GitTag_id(CTX ctx, ksfp_t *sfp _RIX)
{
	git_tag *tag = RawPtr_to(git_tag *, sfp[0]);
	RETURN_(new_ReturnRawPtr(ctx, sfp, (git_oid *)git_tag_id(tag)));
}

/* Fill a list with all the tags in the Repository */
//## @Native @Static Array<String> GitTag.list(GitRepository repo);
KMETHOD GitTag_list(CTX ctx, ksfp_t *sfp _RIX)
{
	git_strarray tag_names;
	git_repository *repo = RawPtr_to(git_repository *, sfp[1]);
	int error = git_tag_list(&tag_names, repo);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_tag_list", error);
		RETURN_(KNH_TNULL(Array));
	}
	kArray *a = new_Array(ctx, CLASS_String, tag_names.count);
	int i;
	for (i = 0; i < tag_names.count; i++) {
		knh_Array_add(ctx, a, tag_names.strings[i]);
	}
	git_strarray_free(&tag_names);
	RETURN_(a);
}

/* Fill a list with all the tags in the Repository which name match a defined
 * pattern */
//## @Native @Static Array<String> GitTag.listMatch(String pattern, GitRepository repo);
KMETHOD GitTag_listMatch(CTX ctx, ksfp_t *sfp _RIX)
{
	git_strarray tag_names;
	const char *pattern = String_to(const char *, sfp[1]);
	git_repository *repo = RawPtr_to(git_repository *, sfp[2]);
	int error = git_tag_list_match(&tag_names, pattern, repo);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_tag_list_match", error);
		RETURN_(KNH_TNULL(Array));
	}
	kArray *a = new_Array(ctx, CLASS_String, tag_names.count);
	int i;
	for (i = 0; i < tag_names.count; i++) {
		knh_Array_add(ctx, a, tag_names.strings[i]);
	}
	git_strarray_free(&tag_names);
	RETURN_(a);
}

/* Lookup a tag object from the repository. */
//## @Native @Static GitTag GitTag.lookup(GitRepository repo, GitOid id);
KMETHOD GitTag_lookup(CTX ctx, ksfp_t *sfp _RIX)
{
	git_tag *tag;
	git_repository *repo = RawPtr_to(git_repository *, sfp[1]);
	const git_oid *id = RawPtr_to(const git_oid *, sfp[2]);
	int error = git_tag_lookup(&tag, repo, id);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_tag_lookup", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, tag));
}

/* Lookup a tag object from the repository, given a prefix of its identifier
 * (short id). */
//## @Native @Static GitTag GitTag.lookupPrefix(GitRepository repo, GitOid id, int len);
KMETHOD GitTag_lookupPrefix(CTX ctx, ksfp_t *sfp _RIX)
{
	git_tag *tag;
	git_repository *repo = RawPtr_to(git_repository *, sfp[1]);
	const git_oid *id = RawPtr_to(const git_oid *, sfp[2]);
	unsigned int len = Int_to(unsigned int, sfp[3]);
	int error = git_tag_lookup_prefix(&tag, repo, id, len);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_tag_lookup_prefix", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, tag));
}

/* Get the message of a tag */
//## @Native String GitTag.message();
KMETHOD GitTag_message(CTX ctx, ksfp_t *sfp _RIX)
{
	git_tag *tag = RawPtr_to(git_tag *, sfp[0]);
	RETURN_(new_String(ctx, git_tag_message(tag)));
}

/* Get the name of a tag */
//## @Native String GitTag.name();
KMETHOD GitTag_name(CTX ctx, ksfp_t *sfp _RIX)
{
	git_tag *tag = RawPtr_to(git_tag *, sfp[0]);
	RETURN_(new_String(ctx, git_tag_name(tag)));
}

/* Get the tagger (author) of a tag */
//## @Native GitSignature GitTag.tagger();
KMETHOD GitTag_tagger(CTX ctx, ksfp_t *sfp _RIX)
{
	git_tag *tag = RawPtr_to(git_tag *, sfp[0]);
	const git_signature *signature = git_tag_tagger(tag);
	RETURN_(new_ReturnRawPtr(ctx, sfp, (git_signature *)signature));
}

/* Get the tagged object of a tag */
//## @Native GitObject GitTag.target();
KMETHOD GitTag_target(CTX ctx, ksfp_t *sfp _RIX)
{
	git_object *target;
	git_tag *tag = RawPtr_to(git_tag *, sfp[0]);
	int error = git_tag_target(&target, tag);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_tag_target", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, target));
}

/* Get the OID of the tagged object of a tag */
//## @Native GitOid GitTag.targetOid();
KMETHOD GitTag_targetOid(CTX ctx, ksfp_t *sfp _RIX)
{
	git_tag *tag = RawPtr_to(git_tag *, sfp[0]);
	const git_oid *id = git_tag_target_oid(tag);
	RETURN_(new_ReturnRawPtr(ctx, sfp, (git_oid *)id));
}

/* Get the type of a tag's tagged object */
//## @Native int GitTag.type();
KMETHOD GitTag_type(CTX ctx, ksfp_t *sfp _RIX)
{
	git_tag *tag = RawPtr_to(git_tag *, sfp[0]);
	RETURNi_(git_tag_type(tag));
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
