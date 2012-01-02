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
#include <konoha1/inlinelibs.h>
#include "libgit2.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------ */

static void kGitRemote_init(CTX ctx, kRawPtr *po)
{
	po->rawptr = NULL;
}

static void kGitRemote_free(CTX ctx, kRawPtr *po)
{
	if (po->rawptr != NULL) {
		git_remote_free((git_remote *)po->rawptr);
		po->rawptr = NULL;
	}
}

DEFAPI(void) defGitRemote(CTX ctx, kclass_t cid, kclassdef_t *cdef)
{
	cdef->name = "GitRemote";
	cdef->init = kGitRemote_init;
	cdef->free = kGitRemote_free;
}

/* ------------------------------------------------------------------------ */

/* Open a connection to a remote */
//## @Native void GitRemote.connect(int direction);
KMETHOD GitRemote_connect(CTX ctx, ksfp_t *sfp _RIX)
{
	struct git_remote *remote = RawPtr_to(struct git_remote *, sfp[0]);
	int direction = Int_to(int, sfp[1]);
	int error = git_remote_connect(remote, direction);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_remote_connect", error);
	}
	RETURNvoid_();
}

/* Download the packfile */
//## @Native String GitRemote.download();
KMETHOD GitRemote_download(CTX ctx, ksfp_t *sfp _RIX)
{
	char *filename;
	git_remote *remote = RawPtr_to(git_remote *, sfp[0]);
	int error = git_remote_download(&filename, remote);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_remote_download", error);
	}
	kString *s = new_String(ctx, filename);
	free(filename);
	RETURN_(s);
}

/* Get the fetch refspec */
//## @Native GitRefspec GitRemote.fetchSpec();
KMETHOD GitRemote_fetchSpec(CTX ctx, ksfp_t *sfp _RIX)
{
	struct git_remote *remote = RawPtr_to(struct git_remote *, sfp[0]);
	const git_refspec *spec = git_remote_fetchspec(remote);
	if (spec == NULL) {
		KNH_NTRACE2(ctx, "git_remote_fetchspec", K_NOTICE, KNH_LDATA(
					LOG_msg("not exists")));
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, (git_refspec *)spec));
}

/* Free the memory associated with a remote */
//## @Native void GitRemote.free();
KMETHOD GitRemote_free(CTX ctx, ksfp_t *sfp _RIX)
{
	kGitRemote_free(ctx, sfp[0].p);
	RETURNvoid_();
}

/* Get the information for a particular remote */
//## @Native GitRemote GitRemote.get(GitConfig cfg, String name);
KMETHOD GitRemote_get(CTX ctx, ksfp_t *sfp _RIX)
{
	struct git_remote *out;
	struct git_config *cfg = RawPtr_to(struct git_config *, sfp[0]);
	const char *name = String_to(const char *, sfp[1]);
	int error = git_remote_get(&out, cfg, name);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_remote_get", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, out));
}

/* Get a list of refs at the remote */
//## @Native Array<String> GitRemote.ls();
KMETHOD GitRemote_ls(CTX ctx, ksfp_t *sfp _RIX)
{
	git_remote *remote = RawPtr_to(git_remote *, sfp[0]);
	git_headarray *refs;
	int error = git_remote_ls(remote, refs);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_remote_ls", error);
		RETURN_(KNH_NULL);
	}
	kArray *a = new_Array(ctx, knh_getcid(ctx, STEXT("GitRemoteHead")), refs->len);
	int i;
	for (i = 0; i < refs->len; i++) {
		knh_Array_add(ctx, a, new_ReturnRawPtr(ctx, sfp, refs->heads[0]));
	}
	RETURN_(a);
}

/* Get the remote's name */
//## @Native String GitRemote.name();
KMETHOD GitRemote_name(CTX ctx, ksfp_t *sfp _RIX)
{
	struct git_remote *remote = RawPtr_to(struct git_remote *, sfp[0]);
	const char *name = git_remote_name(remote);
	RETURN_(new_String(ctx, name));
}

/* Negotiate what data needs to be exchanged to synchroize the remtoe and local
 * references */
//## @Native void GitRemote.negotiate();
KMETHOD GitRemote_negotiate(CTX ctx, ksfp_t *sfp _RIX)
{
	git_remote *remote = RawPtr_to(git_remote *, sfp[0]);
	int error = git_remote_negotiate(remote);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_remote_negotiate", error);
	}
	RETURNvoid_();
}

/* Create a new unnamed remote */
//## @Native @Static GitRemote GitRemote.new(GitRepository repo, String url);
KMETHOD GitRemote_new(CTX ctx, ksfp_t *sfp _RIX)
{
	git_remote *out;
	git_repository *repo = RawPtr_to(git_repository *, sfp[0]);
	const char *url = String_to(const char *, sfp[1]);
	int error = git_remote_new(&out, repo, url);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_remote_new", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, out));
}

/* Get the push refspec */
//## @Native GitRefspec GitRemote.pushspec();
KMETHOD GitRemote_pushspec(CTX ctx, ksfp_t *sfp _RIX)
{
	struct git_remote *remote = RawPtr_to(struct git_remote *, sfp[0]);
	const git_refspec *spec = git_remote_pushspec(remote);
	if (spec == NULL) {
		KNH_NTRACE2(ctx, "git_remote_pushspec", K_NOTICE, KNH_LDATA(
					LOG_msg("not exist")));
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, (git_refspec *)spec));
}

/* Update the tips to the new state */
//## @Native void GitRemote.updateTips();
KMETHOD GitRemote_updateTips(CTX ctx, ksfp_t *sfp _RIX)
{
	struct git_remote *remote = RawPtr_to(struct git_remote *, sfp[0]);
	int error = git_remote_update_tips(remote);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_remote_update_tips", error);
	}
	RETURNvoid_();
}

/* Get the remote's url */
//## @Native String GitRemote.url();
KMETHOD GitRemote_url(CTX ctx, ksfp_t *sfp _RIX)
{
	struct git_remote *remote = RawPtr_to(struct git_remote *, sfp[0]);
	const char *url = git_remote_url(remote);
	RETURN_(new_String(ctx, url));
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
