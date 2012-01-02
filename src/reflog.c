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

static void kGitReflog_init(CTX ctx, kRawPtr *po)
{
	po->rawptr = NULL;
}

static void kGitReflog_free(CTX ctx, kRawPtr *po)
{
	if (po->rawptr != NULL) {
		git_reflog_free((git_reflog *)po->rawptr);
		po->rawptr = NULL;
	}
}

DEFAPI(void) defGitReflog(CTX ctx, kclass_t cid, kclassdef_t *cdef)
{
	cdef->name = "GitReflog";
	cdef->init = kGitReflog_init;
	cdef->free = kGitReflog_free;
}

static void kGitReflogEntry_init(CTX ctx, kRawPtr *po)
{
	po->rawptr = NULL;
}

static void kGitReflogEntry_free(CTX ctx, kRawPtr *po)
{
	if (po->rawptr != NULL) {
		// [TODO]
		po->rawptr = NULL;
	}
}

DEFAPI(void) defGitReflogEntry(CTX ctx, kclass_t cid, kclassdef_t *cdef)
{
	cdef->name = "GitReflogEntry";
	cdef->init = kGitReflogEntry_init;
	cdef->free = kGitReflogEntry_free;
}

/* ------------------------------------------------------------------------ */

/* Lookup an entry by its index */
//## @Native @Static GitReflogEntry GitReflog.entryByIndex(int idx);
KMETHOD GitReflog_entryByIndex(CTX ctx, ksfp_t *sfp _RIX)
{
	git_reflog *reflog = RawPtr_to(git_reflog *, sfp[0]);
	unsigned int idx = Int_to(unsigned int, sfp[1]);
	const git_reflog_entry *entry = git_reflog_entry_byindex(reflog, idx);
	if (entry == NULL) {
		KNH_NTRACE2(ctx, "git_reflog_entry_byindex", K_NOTICE, KNH_LDATA0);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, (git_reflog_entry *)entry));
}

/* Get the committer of this entry */
//## @Native GitSignature GitReflogEntry.comitter();
KMETHOD GitReflogEntry_comitter(CTX ctx, ksfp_t *sfp _RIX)
{
	const git_reflog_entry *entry = RawPtr_to(const git_reflog_entry *, sfp[0]);
	git_signature *committer = git_reflog_entry_committer(entry);
	RETURN_(new_ReturnRawPtr(ctx, sfp, committer));
}

/* Get the log msg */
//## @Native String GitReflogEntry.msg();
KMETHOD GitReflogEntry_msg(CTX ctx, ksfp_t *sfp _RIX)
{
	const git_reflog_entry *entry = RawPtr_to(const git_reflog_entry *, sfp[0]);
	char *msg = git_reflog_entry_msg(entry);
	RETURN_(new_String(ctx, msg));
}

/* Get the new oid */
//## @Native GitOid GitReflogEntry.oidNew();
KMETHOD GitReflogEntry_oidNew(CTX ctx, ksfp_t *sfp _RIX)
{
	const git_reflog_entry *entry = RawPtr_to(const git_reflog_entry *, sfp[0]);
	const git_oid *id = git_reflog_entry_oidnew(entry);
	RETURN_(new_ReturnRawPtr(ctx, sfp, (git_oid *)id));
}

/* Get the old oid */
//## @Native GitOid GitReflogEntry.oidOld();
KMETHOD GitReflogEntry_oidOld(CTX ctx, ksfp_t *sfp _RIX)
{
	const git_reflog_entry *entry = RawPtr_to(const git_reflog_entry *, sfp[0]);
	const git_oid *id = git_reflog_entry_oidold(entry);
	RETURN_(new_ReturnRawPtr(ctx, sfp, (git_oid *)id));
}

/* Get the number of log entries in a reflog */
//## @Native int GitReflog.entryCount();
KMETHOD GitReflog_entryCount(CTX ctx, ksfp_t *sfp _RIX)
{
	git_reflog *reflog = RawPtr_to(git_reflog *, sfp[0]);
	RETURNi_(git_reflog_entrycount(reflog));
}

/* Free the reflog */
//## @Native void GitReflog.free();
KMETHOD GitReflog_free(CTX ctx, ksfp_t *sfp _RIX)
{
	kGitReflog_free(ctx, sfp[0].p);
	RETURNvoid_();
}

/* Read the reflog for the given reference */
//## @Native @Static GitReflog GitReflog.read(GitReference ref);
KMETHOD GitReflog_read(CTX ctx, ksfp_t *sfp _RIX)
{
	git_reflog *reflog;
	git_reference *ref = RawPtr_to(git_reference *, sfp[0]);
	int error = git_reflog_read(&reflog, ref);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_reflog_read", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, reflog));
}

/* Write a new reflog for the given reference */
//## @Native @Static void GitReflog.write(GitReference ref, GitOid oid_old, GitSignature committer, String msg);
KMETHOD GitReflog_write(CTX ctx, ksfp_t *sfp _RIX)
{
	git_reference *ref = RawPtr_to(git_reference *, sfp[1]);
	const git_oid *oid_old = RawPtr_to(const git_oid *, sfp[2]);
	const git_signature *committer = RawPtr_to(const git_signature *, sfp[3]);
	const char *msg = String_to(const char *, sfp[4]);
	int error = git_reflog_write(ref, oid_old, committer, msg);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_reflog_write", error);
	}
	RETURNvoid_();
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
