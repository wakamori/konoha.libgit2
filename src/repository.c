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

#define USE_STRUCT_Path
#include <konoha1.h>
#include "libgit2.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------ */

static void kGitRepository_init(CTX ctx, kRawPtr *po)
{
	po->rawptr = NULL;
}

static void kGitRepository_free(CTX ctx, kRawPtr *po)
{
	if (po->rawptr != NULL) {
		git_repository_free((git_repository *)po->rawptr);
		po->rawptr = NULL;
	}
}

DEFAPI(void) defGitRepository(CTX ctx, kclass_t cid, kclassdef_t *cdef)
{
	cdef->name = "GitRepository";
	cdef->init = kGitRepository_init;
	cdef->free = kGitRepository_free;
}

static knh_IntData_t GitRepositoryConstInt[] = {
	{"PATH", GIT_REPO_PATH},
	{"PATH_INDEX", GIT_REPO_PATH_INDEX},
	{"PATH_ODB", GIT_REPO_PATH_ODB},
	{"PATH_WORKDIR", GIT_REPO_PATH_WORKDIR},
	{NULL}
};

DEFAPI(void) constGitRepository(CTX ctx, kclass_t cid, const knh_LoaderAPI_t *kapi)
{
	kapi->loadClassIntConst(ctx, cid, GitRepositoryConstInt);
}

/* ------------------------------------------------------------------------ */

/* Retrieve the relevant configuration for a repository */
//## @Native GitConfig GitRepository.config(Path global_config_path, Path system_config_path);
KMETHOD GitRepository_config(CTX ctx, ksfp_t *sfp _RIX)
{
	git_config *out;
	git_repository *repo = RawPtr_to(git_repository *, sfp[0]);
	const char *global_config_path = String_to(const char *, sfp[1]);
	const char *system_config_path = String_to(const char *, sfp[2]);
	int error = git_repository_config(&out, repo, global_config_path, system_config_path);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_repository_config", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, out));
}

/* Automatically load the configuration files */
//## @Native GitConfig GitRepository.configAutoload();
KMETHOD GitRepository_configAutoload(CTX ctx, ksfp_t *sfp _RIX)
{
	git_config *out;
	git_repository *repo = RawPtr_to(git_repository *, sfp[0]);
	int error = git_repository_config_autoload(&out, repo);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_repository_config_autoload", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, out));
}

/* Get the object database behind a Git repository */
//## @Native GitOdb GitRepository.database();
KMETHOD GitRepository_database(CTX ctx, ksfp_t *sfp _RIX)
{
	git_repository *repo = RawPtr_to(git_repository *, sfp[0]);
	git_odb *db = git_repository_database(repo);
	RETURN_(new_ReturnRawPtr(ctx, sfp, db));
}

/* Look for a git repository and copy its path in the given buffer. The lookup
 * start from base_path and walk across parent directories if nothing has been
 * found. The lookup ends when the first repository is found, or when reaching a
 * directory referenced in ceiling_dirs or when the filesystem changes (in case
 * across_fs is true). */
//## @Native @Static Path GitRepository.discover(Path start_path, boolean across_fs, Path ceiling_dirs);
KMETHOD GitRepository_discover(CTX ctx, ksfp_t *sfp _RIX)
{
	char repository_path[K_PATHMAX];
	size_t size = K_PATHMAX;
	const char *start_path = sfp[1].pth->ospath;
	int across_fs = Boolean_to(int, sfp[2]);
	const char *ceiling_dirs = String_to(const char *, sfp[3]);
	int error = git_repository_discover(repository_path, size, start_path, across_fs, ceiling_dirs);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_repository_discover", error);
		RETURN_(KNH_TNULL(Path));
	}
	kPath *p = new_Path(ctx, new_String(ctx, repository_path));
	RETURN_(p);
}

/* Free a previously allocated repository */
//## @Native void GitRepository.free();
KMETHOD GitRepository_free(CTX ctx, ksfp_t *sfp _RIX)
{
	kGitRepository_free(ctx, sfp[0].p);
	RETURNvoid_();
}

/* Retrieve and resolve the reference pointed at by HEAD. */
//## @Native GitReference GitRepository.head();
KMETHOD GitRepository_head(CTX ctx, ksfp_t *sfp _RIX)
{
	git_reference *head_out;
	git_repository *repo = RawPtr_to(git_repository *, sfp[0]);
	int error = git_repository_head(&head_out, repo);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_repository_head", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, head_out));
}

/* Check if a repository's HEAD is detached */
//## @Native boolean GitRepository.headDetached();
KMETHOD GitRepository_headDetached(CTX ctx, ksfp_t *sfp _RIX)
{
	git_repository *repo = RawPtr_to(git_repository *, sfp[0]);
	int i = git_repository_head_detached(repo);
	if (i < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_repository_head_detached", i);
		RETURN_(KNH_TNULL(Boolean));
	}
	RETURNb_(i);
}

/* Check if the current branch is an orphan */
//## @Native boolean GitRepository.headOrphan();
KMETHOD GitRepository_headOrphan(CTX ctx, ksfp_t *sfp _RIX)
{
	git_repository *repo = RawPtr_to(git_repository *, sfp[0]);
	int i = git_repository_head_orphan(repo);
	if (i < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_repository_head_orphan", i);
		RETURN_(KNH_TNULL(Boolean));
	}
	RETURNb_(i);
}

/* Open the Index file of a Git repository */
//## @Native GitIndex GitRepository.index();
KMETHOD GitRepository_index(CTX ctx, ksfp_t *sfp _RIX)
{
	git_index *index;
	git_repository *repo = RawPtr_to(git_repository *, sfp[0]);
	int error = git_repository_index(&index, repo);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_repository_index", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, index));
}

/* Creates a new Git repository in the given folder. */
//## @Native @Static GitRepository GitRepository.init(Path path, boolean is_bare);
KMETHOD GitRepository_init(CTX ctx, ksfp_t *sfp _RIX)
{
	git_repository *repo_out;
	const char *path = RawPtr_to(const char *, sfp[1]);
	unsigned is_bare = Boolean_to(unsigned, sfp[2]);
	int error = git_repository_init(&repo_out, path, is_bare);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_repository_init", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, repo_out));
}

/* Check if a repository is bare */
//## @Native boolean GitRepository.isBare();
KMETHOD GitRepository_isBare(CTX ctx, ksfp_t *sfp _RIX)
{
	git_repository *repo = RawPtr_to(git_repository *, sfp[0]);
	RETURNb_(git_repository_is_bare(repo));
}

/* Check if a repository is empty */
//## @Native boolean GitRepository.isEmpty();
KMETHOD GitRepository_isEmpty(CTX ctx, ksfp_t *sfp _RIX)
{
	git_repository *repo = RawPtr_to(git_repository *, sfp[0]);
	RETURNb_(git_repository_is_empty(repo));
}

/* Open a git repository. */
//## @Native @Static GitRepository GitRepository.open(Path path);
KMETHOD GitRepository_open(CTX ctx, ksfp_t *sfp _RIX)
{
	git_repository *repository;
	const char *path = String_to(const char *, sfp[1]);
	int error = git_repository_open(&repository, path);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_repository_open", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, repository));
}

/* Get one of the paths to the repository */
//## @Native Path GitRepository.path(int id);
KMETHOD GitRepository_path(CTX ctx, ksfp_t *sfp _RIX)
{
	git_repository *repo = RawPtr_to(git_repository *, sfp[0]);
	git_repository_pathid id = Int_to(git_repository_pathid, sfp[1]);
	RETURN_(new_String(ctx, git_repository_path(repo, id)));
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
