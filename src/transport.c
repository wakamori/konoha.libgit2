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

static void kGitTransport_init(CTX ctx, kRawPtr *po)
{
	po->rawptr = NULL;
}

static void kGitTransport_free(CTX ctx, kRawPtr *po)
{
	if (po->rawptr != NULL) {
		// [TODO]
		po->rawptr = NULL;
	}
}

DEFAPI(void) defGitTransport(CTX ctx, kclass_t cid, kclassdef_t *cdef)
{
	cdef->name = "GitTransport";
	cdef->init = kGitTransport_init;
	cdef->free = kGitTransport_free;
}

/* ------------------------------------------------------------------------ */

/* Get the appropriate transport for an URL. */
//## @Native GitTransport GitTransport.new(String url);
KMETHOD GitTransport_new(CTX ctx, ksfp_t *sfp _RIX)
{
	git_transport *transport;
	const char *url = String_to(const char *, sfp[1]);
	int error = git_transport_new(&transport, url);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_transport_new", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, transport));
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
