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

static void kGitIndex_init(CTX ctx, kRawPtr *po)
{
	po->rawptr = NULL;
}

static void kGitIndex_free(CTX ctx, kRawPtr *po)
{
	if (po->rawptr != NULL) {
		fprintf(stderr, "git_index_free(%p)\n", po->rawptr);
		git_index_free((git_index *)po->rawptr);
		po->rawptr = NULL;
	}
}

DEFAPI(void) defGitIndex(CTX ctx, kclass_t cid, kclassdef_t *cdef)
{
	cdef->name = "GitIndex";
	cdef->init = kGitIndex_init;
	cdef->free = kGitIndex_free;
}

static void kGitIndexEntry_init(CTX ctx, kRawPtr *po)
{
	po->rawptr = NULL;
}

static void kGitIndexEntry_free(CTX ctx, kRawPtr *po)
{
	if (po->rawptr != NULL) {
		/* do nothing */
		//KNH_FREE(ctx, po->rawptr, sizeof(git_index_entry));
		po->rawptr = NULL;
	}
}

DEFAPI(void) defGitIndexEntry(CTX ctx, kclass_t cid, kclassdef_t *cdef)
{
	cdef->name = "GitIndexEntry";
	cdef->init = kGitIndexEntry_init;
	cdef->free = kGitIndexEntry_free;
}

static void kGitIndexEntryUnmerged_init(CTX ctx, kRawPtr *po)
{
	po->rawptr = NULL;
}

static void kGitIndexEntryUnmerged_free(CTX ctx, kRawPtr *po)
{
	if (po->rawptr != NULL) {
		KNH_FREE(ctx, po->rawptr, sizeof(git_index_entry_unmerged));
		po->rawptr = NULL;
	}
}

DEFAPI(void) defGitIndexEntryUnmerged(CTX ctx, kclass_t cid, kclassdef_t *cdef)
{
	cdef->name = "GitIndexEntryUnmerged";
	cdef->init = kGitIndexEntryUnmerged_init;
	cdef->free = kGitIndexEntryUnmerged_free;
}

/* ------------------------------------------------------------------------ */

/* fields */
//## @Native String GitIndexEntry.getPath();
KMETHOD GitIndexEntry_getPath(CTX ctx, ksfp_t *sfp _RIX)
{
	git_index_entry *entry = RawPtr_to(git_index_entry *, sfp[0]);
	if (index == NULL) {
		RETURN_(KNH_TNULL(String));
	}
	RETURN_(new_String(ctx, entry->path));
}

//## @Native int GitIndexEntry.getMtime();
KMETHOD GitIndexEntry_getMtime(CTX ctx, ksfp_t *sfp _RIX)
{
	git_index_entry *entry = RawPtr_to(git_index_entry *, sfp[0]);
	if (index == NULL) {
		RETURNi_(-1);
	}
	RETURNi_(entry->mtime.seconds);
}

//## @Native int GitIndexEntry.getFileSize();
KMETHOD GitIndexEntry_getFileSize(CTX ctx, ksfp_t *sfp _RIX)
{
	git_index_entry *entry = RawPtr_to(git_index_entry *, sfp[0]);
	if (index == NULL) {
		RETURNi_(-1);
	}
	RETURNi_(entry->file_size);
}

/* Add or update an index entry from a file in disk */
//## @Native void GitIndex.add(Path path, int stage);
KMETHOD GitIndex_add(CTX ctx, ksfp_t *sfp _RIX)
{
	git_index *index = RawPtr_to(git_index *, sfp[0]);
	const char *path = sfp[1].pth->ospath;
	int stage = Int_to(int, sfp[2]);
	int error = git_index_add(index, path, stage);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_index_add", error);
	}
	RETURNvoid_();
}

/* Add (append) an index entry from a file in disk */
//## @Native void GitIndex.append(Path path, int stage);
KMETHOD GitIndex_append(CTX ctx, ksfp_t *sfp _RIX)
{
	git_index *index = RawPtr_to(git_index *, sfp[0]);
	const char *path = sfp[1].pth->ospath;
	int stage = Int_to(int, sfp[2]);
	int error = git_index_append(index, path, stage);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_index_append", error);
	}
	RETURNvoid_();
}

/* Clear the contents (all the entries) of an index object. This clears the
 * index object in memory; changes must be manually written to disk for them to
 * take effect. */
//## @Native void GitIndex.clear();
KMETHOD GitIndex_clear(CTX ctx, ksfp_t *sfp _RIX)
{
	git_index_clear(RawPtr_to(git_index *, sfp[0]));
	RETURNvoid_();
}

/* Return the stage number from a git index entry */
//## @Native @Static int GitIndex.entryStage(GitIndexEntry entry);
KMETHOD GitIndex_entryStage(CTX ctx, ksfp_t *sfp _RIX)
{
	RETURNi_(git_index_entry_stage(RawPtr_to(const git_index_entry *, sfp[1])));
}

/* Get the count of entries currently in the index */
//## @Native int GitIndex.entryCount();
KMETHOD GitIndex_entryCount(CTX ctx, ksfp_t *sfp _RIX)
{
	RETURNi_(git_index_entrycount(RawPtr_to(git_index *, sfp[0])));
}

/* Get the count of unmerged entries currently in the index */
//## @Native int GitIndex.entryCountUnmerged();
KMETHOD GitIndex_entryCountUnmerged(CTX ctx, ksfp_t *sfp _RIX)
{
	RETURNi_(git_index_entrycount_unmerged(RawPtr_to(git_index *, sfp[0])));
}

/* Find the first index of any entries which point to given path in the Git
 * index. */
//## @Native int GitIndex.find(Path path);
KMETHOD GitIndex_find(CTX ctx, ksfp_t *sfp _RIX)
{
	git_index *index = RawPtr_to(git_index *, sfp[0]);
	const char *path = sfp[1].pth->ospath;
	int i = git_index_find(index, path);
	if (i < 0) {
		KNH_NTRACE2(ctx, "git_index_find", K_FAILED, KNH_LDATA0);
		RETURN_(KNH_TNULL(Int));
	}
	RETURNi_(i);
}

/* Free an existing index object. */
//## @Native void GitIndex.free();
KMETHOD GitIndex_free(CTX ctx, ksfp_t *sfp _RIX)
{
	kGitIndex_free(ctx, sfp[0].p);
	RETURNvoid_();
}

/* Get a pointer to one of the entries in the index */
//## @Native GitIndexEntry GitIndex.get(int n);
KMETHOD GitIndex_get(CTX ctx, ksfp_t *sfp _RIX)
{
	git_index *index = RawPtr_to(git_index *, sfp[0]);
	unsigned int n = Int_to(unsigned int, sfp[1]);
	git_index_entry *entry = git_index_get(index, n);
	if (entry == NULL) {
		KNH_NTRACE2(ctx, "git_index_get", K_FAILED, KNH_LDATA(LOG_msg("out of bounds")));
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, entry));
}

/* Get an unmerged entry from the index. */
//## @Native GitIndexEntryUnmerged GitIndex.getUnmergedByIndex(int n);
KMETHOD GitIndex_getUnmergedByIndex(CTX ctx, ksfp_t *sfp _RIX)
{
	git_index *index = RawPtr_to(git_index *, sfp[0]);
	unsigned int n = Int_to(unsigned int, sfp[1]);
	const git_index_entry_unmerged *entry = git_index_get_unmerged_byindex(index, n);
	if (entry == NULL) {
		KNH_NTRACE2(ctx, "git_index_get_unmerged_byindex", K_FAILED, KNH_LDATA(LOG_msg("out of bounds")));
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, (git_index_entry_unmerged *)entry));
}

/* Get an unmerged entry from the index. */
//## @Native GitIndexEntryUnmerged GitIndex.getUnmergedByPath(Path path);
KMETHOD GitIndex_getUnmergedByPath(CTX ctx, ksfp_t *sfp _RIX)
{
	git_index *index = RawPtr_to(git_index *, sfp[0]);
	const char *path = sfp[1].pth->ospath;
	const git_index_entry_unmerged *entry = git_index_get_unmerged_bypath(index, path);
	if (entry == NULL) {
		KNH_NTRACE2(ctx, "git_index_get_unmerged_bypath", K_FAILED, KNH_LDATA(LOG_msg("not found")));
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, (git_index_entry_unmerged *)entry));
}

/* Create a new bare Git index object as a memory representation of the Git
 * index file in 'index_path', without a repository to back it. */
//## @Native @Static GitIndex GitIndex.open(Path index_path);
KMETHOD GitIndex_open(CTX ctx, ksfp_t *sfp _RIX)
{
	git_index *index;
	const char *index_path = S_totext(sfp[1].s);
	int error = git_index_open(&index, index_path);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_index_open", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, index));
}

/* Update the contents of an existing index object in memory by reading from the
 * hard disk. */
//## @Native void GitIndex.read();
KMETHOD GitIndex_read(CTX ctx, ksfp_t *sfp _RIX)
{
	git_index *index = RawPtr_to(git_index *, sfp[0]);
	int error = git_index_read(index);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_index_read", error);
	}
	RETURNvoid_();
}

/* Remove an entry from the index */
//## @Native void GitIndex.remove(int position);
KMETHOD GitIndex_remove(CTX ctx, ksfp_t *sfp _RIX)
{
	git_index *index = RawPtr_to(git_index *, sfp[0]);
	int position = Int_to(int, sfp[1]);
	int error = git_index_remove(index, position);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_index_remove", error);
	}
	RETURNvoid_();
}

/* Remove all entries with equal path except last added */
//## @Native void GitIndex.uniq();
KMETHOD GitIndex_uniq(CTX ctx, ksfp_t *sfp _RIX)
{
	git_index *index = RawPtr_to(git_index *, sfp[0]);
	git_index_uniq(index);
	RETURNvoid_();
}

/* Write an existing index object from memory back to disk using an atomic file
 * lock. */
//## @Native void GitIndex.write();
KMETHOD GitIndex_write(CTX ctx, ksfp_t *sfp _RIX)
{
	git_index *index = RawPtr_to(git_index *, sfp[0]);
	int error = git_index_write(index);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_index_write", error);
	}
	RETURNvoid_();
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
