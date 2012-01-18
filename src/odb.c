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
#include <konoha1/inlinelibs.h>
#include "libgit2.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------ */

static void kGitOdb_init(CTX ctx, kRawPtr *po)
{
	po->rawptr = NULL;
}

static void kGitOdb_free(CTX ctx, kRawPtr *po)
{
	if (po->rawptr != NULL) {
		git_odb_close((git_odb *)po->rawptr);
		po->rawptr = NULL;
	}
}

DEFAPI(void) defGitOdb(CTX ctx, kclass_t cid, kclassdef_t *cdef)
{
	cdef->name = "GitOdb";
	cdef->init = kGitOdb_init;
	cdef->free = kGitOdb_free;
}

static void kGitOdbBackend_init(CTX ctx, kRawPtr *po)
{
	po->rawptr = NULL;
}

static void kGitOdbBackend_free(CTX ctx, kRawPtr *po)
{
	if (po->rawptr != NULL) {
		KNH_FREE(ctx, po->rawptr, sizeof(git_odb_backend));
		po->rawptr = NULL;
	}
}

DEFAPI(void) defGitOdbBackend(CTX ctx, kclass_t cid, kclassdef_t *cdef)
{
	cdef->name = "GitOdbBackend";
	cdef->init = kGitOdbBackend_init;
	cdef->free = kGitOdbBackend_free;
}

static void kGitOdbObject_init(CTX ctx, kRawPtr *po)
{
	po->rawptr = NULL;
}

static void kGitOdbObject_free(CTX ctx, kRawPtr *po)
{
	if (po->rawptr != NULL) {
		git_odb_object_close((git_odb_object *)po->rawptr);
		po->rawptr = NULL;
	}
}

DEFAPI(void) defGitOdbObject(CTX ctx, kclass_t cid, kclassdef_t *cdef)
{
	cdef->name = "GitOdbObject";
	cdef->init = kGitOdbObject_init;
	cdef->free = kGitOdbObject_free;
}

/* ------------------------------------------------------------------------ */

/* Add a custom backend to an existing Object DB; this backend will work as an
 * alternate. */
//## @Native void GitOdb.addAlternate(GitOdbBackend backend, int priority);
KMETHOD GitOdb_addAlternate(CTX ctx, ksfp_t *sfp _RIX)
{
	git_odb *odb = RawPtr_to(git_odb *, sfp[0]);
	git_odb_backend *backend = RawPtr_to(git_odb_backend *, sfp[1]);
	int priority = Int_to(int, sfp[2]);
	int error = git_odb_add_alternate(odb, backend, priority);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_odb_add_alternate", error);
	}
	RETURNvoid_();
}

/* Add a custom backend to an existing Object DB */
//## @Native void GitOdb.addBackend(GitOdbBackend backend, int priority);
KMETHOD GitOdb_addBackend(CTX ctx, ksfp_t *sfp _RIX)
{
	git_odb *odb = RawPtr_to(git_odb *, sfp[0]);
	git_odb_backend *backend = RawPtr_to(git_odb_backend *, sfp[1]);
	int priority = Int_to(int, sfp[2]);
	int error = git_odb_add_backend(odb, backend, priority);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_odb_add_backend", error);
	}
	RETURNvoid_();
}

//## @Native void GitOdbBackend.loose(Path objects_dir, int compression_level, int do_fsync);
KMETHOD GitOdbBackend_loose(CTX ctx, ksfp_t *sfp _RIX)
{
	git_odb_backend *backend_out = RawPtr_to(git_odb_backend *, sfp[0]);
	const char *objects_dir = sfp[1].pth->ospath;
	int compression_level = Int_to(int, sfp[2]);
	int do_fsync = Int_to(int, sfp[3]);
	int error = git_odb_backend_loose(&backend_out, objects_dir, compression_level, do_fsync);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_odb_backend_loose", error);
	}
	RETURNvoid_();
}

/* Streaming mode */
//## @Native void GitOdbBackend.pack(String objects_dir);
KMETHOD GitOdbBackend_pack(CTX ctx, ksfp_t *sfp _RIX)
{
	git_odb_backend *backend_out = RawPtr_to(git_odb_backend *, sfp[0]);
	const char *objects_dir = sfp[1].pth->ospath;
	int error = git_odb_backend_pack(&backend_out, objects_dir);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_odb_backend_pack", error);
	}
	RETURNvoid_();
}

/* Close an open object database. */
//## @Native void GitOdb.close();
KMETHOD GitOdb_close(CTX ctx, ksfp_t *sfp _RIX)
{
	kGitOdb_free(ctx, sfp[0].p);
	RETURNvoid_();
}

/* Determine if the given object can be found in the object database. */
//## @Native boolean GitOdb.exists(GitOid id);
KMETHOD GitOdb_exists(CTX ctx, ksfp_t *sfp _RIX)
{
	git_odb *db = RawPtr_to(git_odb *, sfp[0]);
	const git_oid *id = RawPtr_to(const git_oid *, sfp[1]);
	int i = git_odb_exists(db, id);
	RETURNb_(i);
}

/* Determine the object-ID (sha1 hash) of a data buffer */
//## @Native @Static GitOid GitOdb.hash(Bytes data, int type);
KMETHOD GitOdb_hash(CTX ctx, ksfp_t *sfp _RIX)
{
	git_oid *id = (git_oid *)KNH_MALLOC(ctx, sizeof(git_oid));
	const void *data = BA_totext(sfp[1].ba);
	size_t len = BA_size(sfp[1].ba);
	git_otype type = Int_to(git_otype, sfp[2]);
	int error = git_odb_hash(id, data, len, type);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_odb_hash", error);
		KNH_FREE(ctx, id, sizeof(git_oid));
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, id));
}

/* Read a file from disk and fill a git_oid with the object id that the file
 * would have if it were written to the Object Database as an object of the
 * given type. Similar functionality to git.git's `git hash-object` without the
 * `-w` flag. */
//## @Native @Static GitOid GitOdb.hashfile(Path path, int type);
KMETHOD GitOdb_hashfile(CTX ctx, ksfp_t *sfp _RIX)
{
	git_oid *out = (git_oid *)KNH_MALLOC(ctx, sizeof(git_oid));
	const char *path = sfp[1].pth->ospath;
	git_otype type = RawPtr_to(git_otype, sfp[2]);
	int error = git_odb_hashfile(out, path, type);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_odb_hashfile", error);
		KNH_FREE(ctx, out, sizeof(git_oid));
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, out));
}

/* Create a new object database with no backends. */
//## @Native GitOdb GitOdb.new();
KMETHOD GitOdb_new(CTX ctx, ksfp_t *sfp _RIX)
{
	git_odb *out;
	int error = git_odb_new(&out);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_odb_new", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, out));
}

/* Close an ODB object */
//## @Native void GitOdbObject.close();
KMETHOD GitOdbObject_close(CTX ctx, ksfp_t *sfp _RIX)
{
	kGitOdbObject_free(ctx, sfp[0].p);
	RETURNvoid_();
}

/* Return the data of an ODB object */
//## @Native Bytes GitOdbObject.data();
KMETHOD GitOdbObject_data(CTX ctx, ksfp_t *sfp _RIX)
{
	git_odb_object *object = RawPtr_to(git_odb_object *, sfp[0]);
	if (object == NULL) {
		RETURN_(KNH_TNULL(Bytes));
	}
	const void *data = git_odb_object_data(object);
	size_t size = git_odb_object_size(object);
	kBytes *ba = new_Bytes(ctx, "git_odb_object_data", size);
	knh_Bytes_write2(ctx, ba, data, size);
	RETURN_(ba);
}

/* Return the OID of an ODB object */
//## @Native GitOid GitOdbObject.id();
KMETHOD GitOdbObject_id(CTX ctx, ksfp_t *sfp _RIX)
{
	git_odb_object *object = RawPtr_to(git_odb_object *, sfp[0]);
	const git_oid *id = git_odb_object_id(object);
	RETURN_(new_ReturnRawPtr(ctx, sfp, (git_oid *)id));
}

/* Return the size of an ODB object */
//## @Native int GitOdbObject.size();
KMETHOD GitOdbObject_size(CTX ctx, ksfp_t *sfp _RIX)
{
	git_odb_object *object = RawPtr_to(git_odb_object *, sfp[0]);
	if (object == NULL) {
		RETURNi_(0);
	}
	RETURNi_(git_odb_object_size(object));
}

/* Return the type of an ODB object */
//## @Native int GitOdbObject.type();
KMETHOD GitOdbObject_type(CTX ctx, ksfp_t *sfp _RIX)
{
	git_odb_object *object = RawPtr_to(git_odb_object *, sfp[0]);
	if (object == NULL) {
		RETURNi_(-1);
	}
	RETURNi_(git_odb_object_type(object));
}

/* Create a new object database and automatically add the two default backends:
 * */
//## @Native GitOdb GitOdb.open(Path objects_dir);
KMETHOD GitOdb_open(CTX ctx, ksfp_t *sfp _RIX)
{
	git_odb *out;
	const char *objects_dir = sfp[1].pth->ospath;
	int error = git_odb_open(&out, objects_dir);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_odb_open", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, out));
}

/* Open a stream to read an object from the ODB */
//## @Native GitOdbStream GitOdb.openRstream(GitOid oid);
KMETHOD GitOdb_openRstream(CTX ctx, ksfp_t *sfp _RIX)
{
	git_odb_stream *stream;
	git_odb *db = RawPtr_to(git_odb *, sfp[0]);
	const git_oid *oid = RawPtr_to(const git_oid *, sfp[1]);
	int error = git_odb_open_rstream(&stream, db, oid);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_odb_open_rstream", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, stream));
}

/* Open a stream to write an object into the ODB */
//## @Native GitOdbStream GitOdb.openWstream(int size, int type);
KMETHOD GitOdb_openWstream(CTX ctx, ksfp_t *sfp _RIX)
{
	git_odb_stream *stream;
	git_odb *db = RawPtr_to(git_odb *, sfp[0]);
	int size = Int_to(int, sfp[1]);
	int type = Int_to(int, sfp[2]);
	int error = git_odb_open_wstream(&stream, db, size, type);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_odb_open_wstream", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, stream));
}

/* Read an object from the database. */
//## @Native GitOdbObject GitOdb.read(GitOid id);
KMETHOD GitOdb_read(CTX ctx, ksfp_t *sfp _RIX)
{
	git_odb_object *out;
	git_odb *db = RawPtr_to(git_odb *, sfp[0]);
	const git_oid *id = RawPtr_to(const git_oid *, sfp[1]);
	int error = git_odb_read(&out, db, id);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_odb_read", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, out));
}

/* Read the header of an object from the database, without reading its full
 * contents. */
//## @Native Tuple<int,int> GitOdb.readHeader(GitOid id);
KMETHOD GitOdb_readHeader(CTX ctx, ksfp_t *sfp _RIX)
{
	size_t len_p;
	git_otype type_p;
	git_odb *db = RawPtr_to(git_odb *, sfp[0]);
	const git_oid *id = RawPtr_to(const git_oid *, sfp[1]);
	int error = git_odb_read_header(&len_p, &type_p, db, id);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_odb_read_header", error);
		RETURN_(KNH_NULL);
	}
	kTuple *t = new_ReturnObject(ctx, sfp);
	t->ifields[0] = len_p;
	t->ifields[1] = type_p;
	RETURN_(t);
}

/* Read an object from the database, given a prefix of its identifier. */
//## @Native GitOdbObject GitOdb.readPrefix(GitOid short_id, int len);
KMETHOD GitOdb_readPrefix(CTX ctx, ksfp_t *sfp _RIX)
{
	git_odb_object *out;
	git_odb *db = RawPtr_to(git_odb *, sfp[0]);
	const git_oid *short_id = RawPtr_to(const git_oid *, sfp[1]);
	unsigned int len = Int_to(unsigned int, sfp[2]);
	int error = git_odb_read_prefix(&out, db, short_id, len);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_odb_read_prefix", error);
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, out));
}

/* Write an object directly into the ODB */
//## @Native GitOid GitOdb.write(Bytes data, int type);
KMETHOD GitOdb_write(CTX ctx, ksfp_t *sfp _RIX)
{
	git_oid *oid = (git_oid *)KNH_MALLOC(ctx, sizeof(git_oid));
	git_odb *odb = RawPtr_to(git_odb *, sfp[0]);
	const void *data = BA_totext(sfp[1].ba);
	size_t len = BA_size(sfp[1].ba);
	git_otype type = Int_to(git_otype, sfp[2]);
	int error = git_odb_write(oid, odb, data, len, type);
	if (error < GIT_SUCCESS) {
		TRACE_ERROR(ctx, "git_odb_write", error);
		KNH_FREE(ctx, oid, sizeof(git_oid));
		RETURN_(KNH_NULL);
	}
	RETURN_(new_ReturnRawPtr(ctx, sfp, oid));
}

/* ------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
