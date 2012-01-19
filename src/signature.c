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

static void kGitSignature_init(CTX ctx, kRawPtr *po)
{
	po->rawptr = NULL;
}

static void kGitSignature_free(CTX ctx, kRawPtr *po)
{
	if (po->rawptr != NULL) {
		fprintf(stderr, "git_signature_free(%p)\n", po->rawptr);
		git_signature_free((git_signature *)po->rawptr);
		po->rawptr = NULL;
	}
}

DEFAPI(void) defGitSignature(CTX ctx, kclass_t cid, kclassdef_t *cdef)
{
	cdef->name = "GitSignature";
	cdef->init = kGitSignature_init;
	cdef->free = kGitSignature_free;
}

/* ------------------------------------------------------------------------ */

/* fields */
//## @Native String GitSignature.getName();
KMETHOD GitSignature_getName(CTX ctx, ksfp_t *sfp _RIX)
{
	const git_signature *sig = RawPtr_to(const git_signature *, sfp[0]);
	if (sig == NULL) {
		RETURN_(KNH_TNULL(String));
	}
	RETURN_(new_String(ctx, sig->name));
}

//## @Native String GitSignature.getEmail();
KMETHOD GitSignature_getEmail(CTX ctx, ksfp_t *sfp _RIX)
{
	const git_signature *sig = RawPtr_to(const git_signature *, sfp[0]);
	if (sig == NULL) {
		RETURN_(KNH_TNULL(String));
	}
	RETURN_(new_String(ctx, sig->email));
}

/* Create a copy of an existing signature. */
//## @Native GitSignature GitSignature.dup();
KMETHOD GitSignature_dup(CTX ctx, ksfp_t *sfp _RIX)
{
	const git_signature *sig = RawPtr_to(const git_signature *, sfp[0]);
	RETURN_(new_ReturnRawPtr(ctx, sfp, git_signature_dup(sig)));
}

/* Free an existing signature */
//## @Native void GitSignature.free();
KMETHOD GitSignature_free(CTX ctx, ksfp_t *sfp _RIX)
{
	kGitSignature_free(ctx, sfp[0].p);
	RETURNvoid_();
}

/* Create a new action signature. The signature must be freed manually or using
 * git_signature_free */
//## @Native GitSignature GitSignature.new(String name, String email, int time, int offset);
KMETHOD GitSignature_new(CTX ctx, ksfp_t *sfp _RIX)
{
	git_signature *sig_out;
	const char *name = String_to(const char *, sfp[1]);
	const char *email = String_to(const char *, sfp[2]);
	git_time_t time = Int_to(git_time_t, sfp[3]);
	int offset = Int_to(int, sfp[4]);
	int error = git_signature_new(&sig_out, name, email, time, offset);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_signature_new", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, sig_out));
}

/* Create a new action signature with a timestamp of 'now'. The signature must
 * be freed manually or using git_signature_free */
//## @Native @Static GitSignature GitSignature.now(String name, String email);
KMETHOD GitSignature_now(CTX ctx, ksfp_t *sfp _RIX)
{
	git_signature *sig_out;
	const char *name = String_to(const char *, sfp[1]);
	const char *email = String_to(const char *, sfp[2]);
	int error = git_signature_now(&sig_out, name, email);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_signature_now", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, sig_out));
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
