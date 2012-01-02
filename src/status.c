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

DEFAPI(void) defGitStatus(CTX ctx, kclass_t cid, kclassdef_t *cdef)
{
	cdef->name = "GitStatus";
}

static knh_IntData_t GitStatusConstInt[] = {
	{"CURRENT", GIT_STATUS_CURRENT},
	{"INDEX_NEW", GIT_STATUS_INDEX_NEW},
	{"INDEX_MODIFIED", GIT_STATUS_INDEX_MODIFIED},
	{"INDEX_DELETED", GIT_STATUS_INDEX_DELETED},
	{"WT_NEW", GIT_STATUS_WT_NEW},
	{"WT_MODIFIED", GIT_STATUS_WT_MODIFIED},
	{"WT_DELETED", GIT_STATUS_WT_DELETED},
	{"IGNORED", GIT_STATUS_IGNORED},
	{NULL}
};

DEFAPI(void) constGitStatus(CTX ctx, kclass_t cid, const knh_LoaderAPI_t *kapi)
{
	kapi->loadClassIntConst(ctx, cid, GitStatusConstInt);
}

static int GitStatus_callback(const char *path, unsigned int status, void *payload)
{
	CTX lctx = knh_getCurrentContext();
	ksfp_t *lsfp = lctx->esp;
	KNH_SETv(lctx, lsfp[K_CALLDELTA + 1].o, new_String(lctx, path));
	lsfp[K_CALLDELTA + 2].ivalue = status;
	kFunc *fo = payload;
	knh_Func_invoke(lctx, fo, lsfp, 2);
	return lsfp[K_RTNIDX].ivalue;
}

/* ------------------------------------------------------------------------ */

/* Get file status for a single file */
//## @Native int GitStatus.file(GitRepository repo, Path path);
KMETHOD GitStatus_file(CTX ctx, ksfp_t *sfp _RIX)
{
	unsigned int status_flags;
	git_repository *repo = RawPtr_to(git_repository *, sfp[1]);
	const char *path = sfp[2].pth->ospath;
	int error = git_status_file(&status_flags, repo, path);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_status_file", error);
		RETURN_(KNH_TNULL(Int));
	}
	RETURNi_(status_flags);
}

/* Gather file statuses and run a callback for each one. */
//## @Native void GitStatus.foreach(GitRepository repo, Func<Path,int=>int> callback);
KMETHOD GitStatus_foreach(CTX ctx, ksfp_t *sfp _RIX)
{
	git_repository *repo = RawPtr_to(git_repository *, sfp[1]);
	int (*callback)(const char *, unsigned int, void *) = GitStatus_callback;
	void *payload = sfp[2].fo;
	int error = git_status_foreach(repo, callback, payload);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_status_foreach", error);
	}
	RETURNvoid_();
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
