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

static void kGitConfig_init(CTX ctx, kRawPtr *po)
{
	po->rawptr = NULL;
}

static void kGitConfig_free(CTX ctx, kRawPtr *po)
{
	if (po->rawptr != NULL) {
		git_config_free((git_config *)po->rawptr);
		po->rawptr = NULL;
	}
}

DEFAPI(void) defGitConfig(CTX ctx, kclass_t cid, kclassdef_t *cdef)
{
	cdef->name = "GitConfig";
	cdef->init = kGitConfig_init;
	cdef->free = kGitConfig_free;
}

static void kGitConfigFile_init(CTX ctx, kRawPtr *po)
{
	po->rawptr = NULL;
}

static void kGitConfigFile_free(CTX ctx, kRawPtr *po)
{
	if (po->rawptr != NULL) {
		KNH_FREE(ctx, po->rawptr, sizeof(git_config_file));
		po->rawptr = NULL;
	}
}

DEFAPI(void) defGitConfigFile(CTX ctx, kclass_t cid, kclassdef_t *cdef)
{
	cdef->name = "GitConfigFile";
	cdef->init = kGitConfigFile_init;
	cdef->free = kGitConfigFile_free;
}

static int callback_func(const char *var_name, const char *value, void *payload)
{
	CTX lctx = knh_getCurrentContext();
	ksfp_t *lsfp = lctx->esp;
	KNH_SETv(lctx, lsfp[K_CALLDELTA + 1].o, new_String(lctx, var_name));
	KNH_SETv(lctx, lsfp[K_CALLDELTA + 2].o, new_String(lctx, value));
	kFunc *fo = (kFunc *)payload;
	knh_Func_invoke(lctx, fo, lsfp, 2);
	return lsfp[K_RTNIDX].ivalue;
}

/* ------------------------------------------------------------------------ */

/* Add a generic config file instance to an existing config */
//## @Native void GitConfig.addFile(GitConfigFile file, int priority);
KMETHOD GitConfig_addFile(CTX ctx, ksfp_t *sfp _RIX)
{
	git_config *cfg = RawPtr_to(git_config *, sfp[0]);
	git_config_file *file = RawPtr_to(git_config_file *, sfp[1]);
	int priority = Int_to(int, sfp[2]);
	int error = git_config_add_file(cfg, file, priority);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_config_add_file", error);
	}
	RETURNvoid_();
}

/* Add an on-disk config file instance to an existing config */
//## @Native void GitConfig.addFileOndisk(Path path, int priority);
KMETHOD GitConfig_addFileOndisk(CTX ctx, ksfp_t *sfp _RIX)
{
	git_config *cfg = RawPtr_to(git_config *, sfp[0]);
	const char *path = sfp[1].pth->ospath;
	int priority = Int_to(int, sfp[2]);
	int error = git_config_add_file_ondisk(cfg, path, priority);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_config_add_file_ondisk", error);
	}
	RETURNvoid_();
}

/* Delete a config variable */
//## @Native void GitConfig.delete(String name);
KMETHOD GitConfig_delete(CTX ctx, ksfp_t *sfp _RIX)
{
	git_config *cfg = RawPtr_to(git_config *, sfp[0]);
	const char *name = RawPtr_to(const char *, sfp[1]);
	int error = git_config_delete(cfg, name);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_config_delete", error);
	}
	RETURNvoid_();
}

/* Create a configuration file backend for ondisk files */
//## @Native @Static GitConfigFile GitConfig.fileOndisk(Path path);
KMETHOD GitConfig_fileOndisk(CTX ctx, ksfp_t *sfp _RIX)
{
	struct git_config_file *out;
	const char *path = sfp[1].pth->ospath;
	int error = git_config_file__ondisk(&out, path);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_config_file__ondisk", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, out));
}

/* Locate the path to the global configuration file */
//## @Native Path GitConfig.findGlobal();
KMETHOD GitConfig_findGlobal(CTX ctx, ksfp_t *sfp _RIX)
{
	char *global_config_path;
	int error = git_config_find_global(global_config_path);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_config_find_global", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_Path(ctx, new_String(ctx, global_config_path)));
}

/* Locate the path to the system configuration file */
//## @Native Path GitConfig.findSystem();
KMETHOD GitConfig_findSystem(CTX ctx, ksfp_t *sfp _RIX)
{
	char *system_config_path;
	int error = git_config_find_system(system_config_path);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_config_find_system", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_Path(ctx, new_String(ctx, system_config_path)));
}

/* Perform an operation on each config variable. */
//## @Native void GitConfig.foreach(Func<String,String=>int> callback);
KMETHOD GitConfig_foreach(CTX ctx, ksfp_t *sfp _RIX)
{
	git_config *cfg = RawPtr_to(git_config *, sfp[0]);
	int (*callback)(const char *, const char *, void *) = callback_func;
	void *payload = sfp[1].fo;
	int error = git_config_foreach(cfg, callback, payload);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_config_foreach", error);
	}
	RETURNvoid_();
}

/* Free the configuration and its associated memory and files */
//## @Native void GitConfig.free();
KMETHOD GitConfig_free(CTX ctx, ksfp_t *sfp _RIX)
{
	kGitConfig_free(ctx, sfp[0].p);
	RETURNvoid_();
}

/* Get the value of a boolean config variable. */
//## @Native boolean GitConfig.getBool(String name);
KMETHOD GitConfig_getBool(CTX ctx, ksfp_t *sfp _RIX)
{
	git_config *cfg = RawPtr_to(git_config *, sfp[0]);
	const char *name = S_totext(sfp[1].s);
	int *out;
	int error = git_config_get_bool(cfg, name, out);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_config_get_bool", error);
		RETURNb_(0);
	}
	RETURNb_(*out == 0 ? 0 : 1);
}

/* Get the value of a string config variable. */
//## @Native String GitConfig.getString(String name);
KMETHOD GitConfig_getString(CTX ctx, ksfp_t *sfp _RIX)
{
	git_config *cfg = RawPtr_to(git_config *, sfp[0]);
	const char *name = S_totext(sfp[1].s);
	const char *out;
	int error = git_config_get_string(cfg, name, &out);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_config_get_string", error);
		RETURN_(KNH_TNULL(String));
	}
	RETURN_(new_String(ctx, out));
}

/* Allocate a new configuration object */
//## @Native GitConfig GitConfig.new();
KMETHOD GitConfig_new(CTX ctx, ksfp_t *sfp _RIX)
{
	git_config *out = RawPtr_to(git_config *, sfp[0]);
	int error = git_config_new(&out);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_config_new", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, out));
}

/* Open the global configuration file */
//## @Native @Static GitConfig GitConfig.openGlobal();
KMETHOD GitConfig_openGlobal(CTX ctx, ksfp_t *sfp _RIX)
{
	git_config *out = RawPtr_to(git_config *, sfp[0]);
	int error = git_config_open_global(&out);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_config_open_global", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, out));
}

/* Create a new config instance containing a single on-disk file */
//## @Native @Static GitConfig GitConfig.openOndisk(String path);
KMETHOD GitConfig_openOndisk(CTX ctx, ksfp_t *sfp _RIX)
{
	git_config *cfg;
	const char *path = S_totext(sfp[1].s);
	int error = git_config_open_ondisk(&cfg, path);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_config_open_ondisk", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, cfg));
}

/* Set the value of a boolean config variable. */
//## @Native void GitConfig.setBool(String name, boolean value);
KMETHOD GitConfig_setBool(CTX ctx, ksfp_t *sfp _RIX)
{
	git_config *cfg = RawPtr_to(git_config *, sfp[0]);
	const char *name = S_totext(sfp[1].s);
	int value = Boolean_to(int ,sfp[2]);
	int error = git_config_set_bool(cfg, name, value);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_config_set_bool", error);
	}
	RETURNvoid_();
}

/* Set the value of a string config variable. */
//## @Native void GitConfig.setString(String name, String value);
KMETHOD GitConfig_setString(CTX ctx, ksfp_t *sfp _RIX)
{
	git_config *cfg = RawPtr_to(git_config *, sfp[0]);
	const char *name = S_totext(sfp[1].s);
	const char *value = S_totext(sfp[2].s);
	int error = git_config_set_string(cfg, name, value);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_config_set_string", error);
	}
	RETURNvoid_();
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
