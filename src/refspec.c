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

static void kGitRefspec_init(CTX ctx, kRawPtr *po)
{
	po->rawptr = NULL;
}

static void kGitRefspec_free(CTX ctx, kRawPtr *po)
{
	if (po->rawptr != NULL) {
		//KNH_FREE(ctx, po->rawptr, sizeof(void *) * 3 + sizeof(int) * 3);
		// [TODO]
		po->rawptr = NULL;
	}
}

DEFAPI(void) defGitRefspec(CTX ctx, kclass_t cid, kclassdef_t *cdef)
{
	cdef->name = "GitRefspec";
	cdef->init = kGitRefspec_init;
	cdef->free = kGitRefspec_free;
}

/* ------------------------------------------------------------------------ */

/* Match a refspec's source descriptor with a reference name */
//## @Native void GitRefspec.srcMatch(String refname);
KMETHOD GitRefspec_srcMatch(CTX ctx, ksfp_t *sfp _RIX)
{
	const git_refspec *refspec = RawPtr_to(const git_refspec *, sfp[0]);
	const char *refname = String_to(const char *, sfp[1]);
	int error = git_refspec_src_match(refspec, refname);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_refspec_src_match", error);
	}
	RETURNvoid_();
}

/* Transform a reference to its target following the refspec's rules */
//## @Native void GitRefspec.transform(Bytes out, String name);
KMETHOD GitRefspec_transform(CTX ctx, ksfp_t *sfp _RIX)
{
	const git_refspec *spec = RawPtr_to(const git_refspec *, sfp[0]);
	char *out = BA_tobytes(sfp[1].ba).buf;
	size_t outlen = BA_size(sfp[1].ba);
	const char *name = String_to(const char *, sfp[2]);
	int error = git_refspec_transform(out, outlen, spec, name);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_refspec_transform", error);
	}
	RETURNvoid_();
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
