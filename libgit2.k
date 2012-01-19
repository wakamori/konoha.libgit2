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

include "lib:gluelink";

/* ------------------------------------------------------------------------ */
// [classes]
@Native class GitBlob;
@Native class GitCommit;
@Native class GitConfig;
@Native class GitConfigFile;
@Native class GitIndex;
@Native class GitIndexEntry;
@Native class GitIndexEntryUnmerged;
@Native class GitIndexer;
@Native class GitIndexerStats;
@Native class GitObject;
@Native class GitOdb;
@Native class GitOdbBackend;
@Native class GitOdbObject;
@Native class GitOid;
@Native class GitOidShorten;
@Native class GitReference;
@Native class GitReflog;
@Native class GitReflogEntry;
@Native class GitRefspec;
@Native class GitRemote;
@Native class GitRepository;
@Native class GitRevwalk;
@Native class GitSort;
@Native class GitSignature;
@Native class GitStatus;
@Native class GitTag;
@Native class GitTransport;
@Native class GitTree;
@Native class GitTreeEntry;
@Native class GitTreebuilder;

/* ------------------------------------------------------------------------ */
// [blob]

/* Close an open blob */
@Native void GitBlob.close();

/* Write an in-memory buffer to the ODB as a blob */
@Native @Static GitOid GitBlob.createFromBuffer(GitRepository repo, Bytes buffer);

/* Read a file from the working folder of a repository and write it to the
 * Object Database as a loose blob */
@Native @Static GitOid GitBlob.createFromFile(GitRepository repo, String path);

/* Lookup a blob object from a repository */
@Native @Static GitBlob GitBlob.lookup(GitRepository repo, GitOid id);

/* Lookup a blob object from a repository, given a prefix of its identifier
 * (short id). */
@Native @Static GitBlob GitBlob.lookupPrefix(GitRepository repo, GitOid id, int len);

/* Get a read-only buffer with the raw content of a blob. */
@Native Bytes GitBlob.rawContent();

/* Get the size in bytes of the contents of a blob */
@Native int GitBlob.rawSize();

/* ------------------------------------------------------------------------ */
// [commit]

/* Get the author of a commit. */
@Native GitSignature GitCommit.author();

/* Close an open commit */
@Native void GitCommit.close();

/* Get the committer of a commit. */
@Native GitSignature GitCommit.committer();

/* Create a new commit in the repository using `git_object` instances as
 * parameters. */
@Native @Static GitOid GitCommit.create(GitRepository repo, String update_ref, GitSignature author, GitSignature committer, String message_encoding, String message, GitTree tree, Array<GitCommit> parents);

/* Create a new commit in the repository using a variable argument list. */
//@Native @Static GitOid GitCommit.createV(GitRepository repo, String update_ref, GitSignature author, GitSignature committer, String message_encoding, String message, GitTree tree, ...);

/* Get the id of a commit. */
@Native GitOid GitCommit.id();

/* Lookup a commit object from a repository. */
@Native @Static GitCommit GitCommit.lookup(GitRepository repo, GitOid id);

/* Lookup a commit object from a repository, given a prefix of its identifier
 * (short id). */
@Native @Static GitCommit GitCommit.lookupPrefix(GitRepository repo, GitOid id, int len);

/* Get the full message of a commit. */
@Native String GitCommit.message();

/* Get the encoding for the message of a commit, as a string representing a
 * standard encoding name. */
@Native String GitCommit.messageEncoding();

/* Get the specified parent of the commit. */
@Native GitCommit GitCommit.parent(int n);

/* Get the oid of a specified parent for a commit. This is different from
 * `git_commit_parent`, which will attempt to load the parent commit from the
 * ODB. */
@Native GitOid GitCommit.parentOid(int n);

/* Get the number of parents of this commit */
@Native int GitCommit.parentCount();

/* Get the commit time (i.e. committer time) of a commit. */
@Native int GitCommit.time();

/* Get the commit timezone offset (i.e. committer's preferred timezone) of a
 * commit. */
@Native int GitCommit.timeOffset();

/* Get the tree pointed to by a commit. */
@Native GitTree GitCommit.tree();

/* Get the id of the tree pointed to by a commit. This differs from
 * `git_commit_tree` in that no attempts are made to fetch an object from the
 * ODB. */
@Native GitOid GitCommit.treeOid();

/* ------------------------------------------------------------------------ */
// [config]

/* Add a generic config file instance to an existing config */
@Native void GitConfig.addFile(GitConfigFile file, int priority);

/* Add an on-disk config file instance to an existing config */
@Native void GitConfig.addFileOndisk(Path path, int priority);

/* Delete a config variable */
@Native void GitConfig.delete(String name);

/* Create a configuration file backend for ondisk files */
@Native @Static GitConfigFile GitConfig.fileOndisk(Path path);

/* Locate the path to the global configuration file */
@Native Path GitConfig.findGlobal();

/* Locate the path to the system configuration file */
@Native Path GitConfig.findSystem();

/* Perform an operation on each config variable. */
@Native void GitConfig.foreach(Func<String,String=>int> callback);

/* Free the configuration and its associated memory and files */
@Native void GitConfig.free();

/* Get the value of a boolean config variable. */
@Native boolean GitConfig.getBool(String name);

/* Get the value of a string config variable. */
@Native String GitConfig.getString(String name);

/* Allocate a new configuration object */
@Native GitConfig GitConfig.new();

/* Open the global configuration file */
@Native @Static GitConfig GitConfig.openGlobal();

/* Create a new config instance containing a single on-disk file */
@Native @Static GitConfig GitConfig.openOndisk(String path);

/* Set the value of a boolean config variable. */
@Native void GitConfig.setBool(String name, boolean value);

/* Set the value of a string config variable. */
@Native void GitConfig.setString(String name, String value);

/* ------------------------------------------------------------------------ */
// [index]

/* fields */
@Native String GitIndexEntry.getPath();
@Native int GitIndexEntry.getMtime();
@Native int GitIndexEntry.getFileSize();

/* Add or update an index entry from a file in disk */
@Native void GitIndex.add(Path path, int stage);

/* Add (append) an index entry from a file in disk */
@Native void GitIndex.append(Path path, int stage);

/* Clear the contents (all the entries) of an index object. This clears the
 * index object in memory; changes must be manually written to disk for them to
 * take effect. */
@Native void GitIndex.clear();

/* Return the stage number from a git index entry */
@Native @Static int GitIndex.entryStage(GitIndexEntry entry);

/* Get the count of entries currently in the index */
@Native int GitIndex.entryCount();

/* Get the count of unmerged entries currently in the index */
@Native int GitIndex.entryCountUnmerged();

/* Find the first index of any entries which point to given path in the Git
 * index. */
@Native int GitIndex.find(Path path);

/* Free an existing index object. */
@Native void GitIndex.free();

/* Get a pointer to one of the entries in the index */
@Native GitIndexEntry GitIndex.get(int n);

/* Get an unmerged entry from the index. */
@Native GitIndexEntryUnmerged GitIndex.getUnmergedByIndex(int n);

/* Get an unmerged entry from the index. */
@Native GitIndexEntryUnmerged GitIndex.getUnmergedByPath(Path path);

/* Create a new bare Git index object as a memory representation of the Git
 * index file in 'index_path', without a repository to back it. */
@Native @Static GitIndex GitIndex.open(Path index_path);

/* Update the contents of an existing index object in memory by reading from the
 * hard disk. */
@Native void GitIndex.read();

/* Remove an entry from the index */
@Native void GitIndex.remove(int position);

/* Remove all entries with equal path except last added */
@Native void GitIndex.uniq();

/* Write an existing index object from memory back to disk using an atomic file
 * lock. */
@Native void GitIndex.write();

/* ------------------------------------------------------------------------ */
// [indexer]

/* Free the indexer and its resources */
@Native void GitIndexer.free();

/* Get the packfile's hash */
@Native GitOid GitIndexer.hash();

/* Create a new indexer instance */
@Native GitIndexer GitIndexer.new(String packname);

/* Iterate over the objects in the packfile and extract the information */
@Native GitIndexerStats GitIndexer.run();

/* Write the index file to disk. */
@Native void GitIndexer.write();

/* ------------------------------------------------------------------------ */
// [object]

/* Get the size in bytes for the structure which acts as an in-memory
 * representation of any given object type. */
@Native @Static int GitObject.size(int type);

/* Close an open object */
@Native void GitObject.close();

/* Get the id (SHA1) of a repository object */
@Native GitOid GitObject.id();

/* Lookup a reference to one of the objects in a repostory. */
@Native @Static GitObject GitObject.lookup(GitRepository repo, GitOid id, int type);

/* Lookup a reference to one of the objects in a repostory, given a prefix of
 * its identifier (short id). */
@Native @Static GitObject GitObject.lookupPrefix(GitRepository repo, GitOid id, int len, int type);

/* Get the repository that owns this object */
@Native GitRepository GitObject.owner();

/* Get the object type of an object */
@Native int GitObject.type();

/* Determine if the given git_otype is a valid loose object type. */
@Native @Static boolean GitObject.typeisLoose(int type);

/* ------------------------------------------------------------------------ */
// [odb]

/* Add a custom backend to an existing Object DB; this backend will work as an
 * alternate. */
@Native void GitOdb.addAlternate(GitOdbBackend backend, int priority);

/* Add a custom backend to an existing Object DB */
@Native void GitOdb.addBackend(GitOdbBackend backend, int priority);

@Native void GitOdbBackend.loose(Path objects_dir, int compression_level, int do_fsync);

/* Streaming mode */
@Native void GitOdbBackend.pack(String objects_dir);

/* Close an open object database. */
@Native void GitOdb.close();

/* Determine if the given object can be found in the object database. */
@Native boolean GitOdb.exists(GitOid id);

/* Determine the object-ID (sha1 hash) of a data buffer */
@Native @Static GitOid GitOdb.hash(Bytes data, int type);

/* Read a file from disk and fill a git_oid with the object id that the file
 * would have if it were written to the Object Database as an object of the
 * given type. Similar functionality to git.git's `git hash-object` without the
 * `-w` flag. */
@Native @Static GitOid GitOdb.hashfile(Path path, int type);

/* Create a new object database with no backends. */
@Native GitOdb GitOdb.new();

/* Close an ODB object */
@Native void GitOdbObject.close();

/* Return the data of an ODB object */
@Native Bytes GitOdbObject.data();

/* Return the OID of an ODB object */
@Native GitOid GitOdbObject.id();

/* Return the size of an ODB object */
@Native int GitOdbObject.size();

/* Return the type of an ODB object */
@Native int GitOdbObject.type();

/* Create a new object database and automatically add the two default backends:
 * */
@Native GitOdb GitOdb.open(Path objects_dir);

/* Open a stream to read an object from the ODB */
@Native GitOdbStream GitOdb.openRstream(GitOid oid);

/* Open a stream to write an object into the ODB */
@Native GitOdbStream GitOdb.openWstream(int size, int type);

/* Read an object from the database. */
@Native GitOdbObject GitOdb.read(GitOid id);

/* Read the header of an object from the database, without reading its full
 * contents. */
@Native Tuple<int,int> GitOdb.readHeader(GitOid id);

/* Read an object from the database, given a prefix of its identifier. */
@Native GitOdbObject GitOdb.readPrefix(GitOid short_id, int len);

/* Write an object directly into the ODB */
@Native GitOid GitOdb.write(Bytes data, int type);

/* ------------------------------------------------------------------------ */
// [oid]

/* fields */
@Native String GitOid.getId();

/* Format a git_oid into a newly allocated c-string. */
@Native String GitOid.allocfmt();

/* Copy an oid from one structure to another. */
@Native GitOid GitOid.cpy();

/* Format a git_oid into a hex string. */
@Native String GitOid.fmt();

/* Copy an already raw oid into a git_oid structure. */
@Native @Static GitOid GitOid.fromRaw(Bytes raw);

/* Parse a hex formatted object id into a git_oid. */
@Native @Static GitOid GitOid.fromStr(String str);

/* Parse N characters of a hex formatted object id into a git_oid */
@Native @Static GitOid GitOid.fromStrn(String str, int length);

/* Compare the first 'len' hexadecimal characters (packets of 4 bits) of two oid
 * structures. */
@Native int GitOid.ncmp(GitOid a, GitOid b, int len);

/* Format a git_oid into a loose-object path string. */
@Native String GitOid.pathfmt();

/* Add a new OID to set of shortened OIDs and calculate the minimal length to
 * uniquely identify all the OIDs in the set. */
@Native void GitOidShorten.add(String text_oid);

/* Free an OID shortener instance */
@Native void GitOidShorten.free();

/* Format a git_oid into a buffer as a hex format c-string. */
@Native String GitOid.toString(int n);

/* ------------------------------------------------------------------------ */
// [reference]

/* Create a new object id reference. */
@Native @Static GitReference GitReference.createOid(GitRepository repo, String name, GitOid id, int force);

/* Create a new symbolic reference. */
@Native @Static GitReference GitReference.createSymbolic(GitRepository repo, String name, String target, int force);

/* Delete an existing reference */
@Native void GitReference.delete();

/* Perform an operation on each reference in the repository */
@Native @Static void GitReference.foreach(GitRepository repo, int list_flags, Func<String=>int> callback);

/* Fill a list with all the references that can be found in a repository. */
@Native @Static Array<String> GitReference.listAll(GitRepository repo, int list_flags);

/* Lookup a reference by its name in a repository. */
@Native @Static GitReference GitReference.lookup(GitRepository repo, String name);

/* Get the full name of a reference */
@Native String GitReference.name();

/* Get the OID pointed to by a reference. */
@Native GitOid GitReference.oid();

/* Get the repository where a reference resides */
@Native GitRepository GitReference.owner();

/* Pack all the loose references in the repository */
@Native void GitReference.packAll();

/* Rename an existing reference */
@Native void GitReference.rename(String new_name, int force);

/* Resolve a symbolic reference */
@Native GitReference GitReference.resolve();

/* Set the OID target of a reference. */
@Native void GitReference.setOid(GitOid id);

/* Set the symbolic target of a reference. */
@Native void GitReference.setTarget(String target);

/* Get full name to the reference pointed by this reference */
@Native String GitReference.target();

/* Get the type of a reference */
@Native int GitReference.type();

/* ------------------------------------------------------------------------ */
// [reflog]

/* Lookup an entry by its index */
@Native @Static GitReflogEntry GitReflog.entryByIndex(int idx);

/* Get the committer of this entry */
@Native GitSignature GitReflogEntry.comitter();

/* Get the log msg */
@Native String GitReflogEntry.msg();

/* Get the new oid */
@Native GitOid GitReflogEntry.oidNew();

/* Get the old oid */
@Native GitOid GitReflogEntry.oidOld();

/* Get the number of log entries in a reflog */
@Native int GitReflog.entryCount();

/* Free the reflog */
@Native void GitReflog.free();

/* Read the reflog for the given reference */
@Native @Static GitReflog GitReflog.read(GitReference ref);

/* Write a new reflog for the given reference */
@Native @Static void GitReflog.write(GitReference ref, GitOid oid_old, GitSignature committer, String msg);

/* ------------------------------------------------------------------------ */
// [refspec]

/* Match a refspec's source descriptor with a reference name */
@Native void GitRefspec.srcMatch(String refname);

/* Transform a reference to its target following the refspec's rules */
@Native void GitRefspec.transform(Bytes out, String name);

/* ------------------------------------------------------------------------ */
// [remote]

/* Open a connection to a remote */
@Native void GitRemote.connect(int direction);

/* Download the packfile */
@Native String GitRemote.download();

/* Get the fetch refspec */
@Native GitRefspec GitRemote.fetchSpec();

/* Free the memory associated with a remote */
@Native void GitRemote.free();

/* Get the information for a particular remote */
@Native GitRemote GitRemote.get(GitConfig cfg, String name);

/* Get a list of refs at the remote */
@Native Array<String> GitRemote.ls();

/* Get the remote's name */
@Native String GitRemote.name();

/* Negotiate what data needs to be exchanged to synchroize the remtoe and local
 * references */
@Native void GitRemote.negotiate();

/* Create a new unnamed remote */
@Native @Static GitRemote GitRemote.new(GitRepository repo, String url);

/* Get the push refspec */
@Native GitRefspec GitRemote.pushspec();

/* Update the tips to the new state */
@Native void GitRemote.updateTips();

/* Get the remote's url */
@Native String GitRemote.url();

/* ------------------------------------------------------------------------ */
// [repository]

/* ------------------------------------------------------------------------ */
/* Retrieve the relevant configuration for a repository */
@Native GitConfig GitRepository.config(Path global_config_path, Path system_config_path);

/* Automatically load the configuration files */
@Native GitConfig GitRepository.configAutoload();

/* Get the object database behind a Git repository */
@Native GitOdb GitRepository.database();

/* Look for a git repository and copy its path in the given buffer. The lookup
 * start from base_path and walk across parent directories if nothing has been
 * found. The lookup ends when the first repository is found, or when reaching a
 * directory referenced in ceiling_dirs or when the filesystem changes (in case
 * across_fs is true). */
@Native @Static Path GitRepository.discover(Path start_path, boolean across_fs, Path ceiling_dirs);

/* Free a previously allocated repository */
@Native void GitRepository.free();

/* Retrieve and resolve the reference pointed at by HEAD. */
@Native GitReference GitRepository.head();

/* Check if a repository's HEAD is detached */
@Native boolean GitRepository.headDetached();

/* Check if the current branch is an orphan */
@Native boolean GitRepository.headOrphan();

/* Open the Index file of a Git repository */
@Native GitIndex GitRepository.index();

/* Creates a new Git repository in the given folder. */
@Native @Static GitRepository GitRepository.init(Path path, boolean is_bare);

/* Check if a repository is bare */
@Native boolean GitRepository.isBare();

/* Check if a repository is empty */
@Native boolean GitRepository.isEmpty();

/* Open a git repository. */
@Native @Static GitRepository GitRepository.open(Path path);

/* Get one of the paths to the repository */
@Native Path GitRepository.path(int id);

/* ------------------------------------------------------------------------ */
// [revwalk]

/* Free a revision walker previously allocated. */
@Native void GitRevwalk.free();

/* Mark a commit (and its ancestors) uninteresting for the output. */
@Native void GitRevwalk.hide(GitOid oid);

/* Allocate a new revision walker to iterate through a repo. */
@Native GitRevwalk GitRevwalk.new(GitRepository repo);

/* Get the next commit from the revision walk. */
@Native GitOid GitRevwalk.next();

/* Mark a commit to start traversal from. */
@Native void GitRevwalk.push(GitOid oid);

/* Return the repository on which this walker is operating. */
@Native GitRepository GitRevwalk.repository();

/* Reset the revision walker for reuse. */
@Native void GitRevwalk.reset();

/* Change the sorting mode when iterating through the repository's contents. */
@Native void GitRevwalk.sorting(int sort_mode);

/* ------------------------------------------------------------------------ */
// [signature]

/* fields */
@Native String GitSignature.getName();
@Native String GitSignature.getEmail();

/* Create a copy of an existing signature. */
@Native GitSignature GitSignature.dup();

/* Free an existing signature */
@Native void GitSignature.free();

/* Create a new action signature. The signature must be freed manually or using
 * git_signature_free */
@Native GitSignature GitSignature.new(String name, String email, int time, int offset);

/* Create a new action signature with a timestamp of 'now'. The signature must
 * be freed manually or using git_signature_free */
@Native @Static GitSignature GitSignature.now(String name, String email);

/* ------------------------------------------------------------------------ */
// [status]

/* Get file status for a single file */
@Native int GitStatus.file(GitRepository repo, Path path);

/* Gather file statuses and run a callback for each one. */
@Native void GitStatus.foreach(GitRepository repo, Func<Path,int=>int> callback);

/* ------------------------------------------------------------------------ */
// [tag]

/* Close an open tag */
@Native void GitTag.close();

/* Create a new tag in the repository from an object */
@Native @Static GitOid GitTag.create(GitRepository repo, String tag_name, GitObject target, GitSignature tagger, String message, int force);

/* Create a new tag in the repository from a buffer */
@Native @Static GitOid GitTag.createFromBuffer(GitRepository repo, Bytes buffer, int force);

/* Create a new lightweight tag pointing at a target object */
@Native GitOid GitTag.createLightweight(GitRepository repo, String tag_name, GitObject target, int force);

/* Delete an existing tag reference. */
@Native @Static void GitTag.delete(GitRepository repo, String tag_name);

/* Get the id of a tag. */
@Native GitOid GitTag.id();

/* Fill a list with all the tags in the Repository */
@Native @Static Array<String> GitTag.list(GitRepository repo);

/* Fill a list with all the tags in the Repository which name match a defined
 * pattern */
@Native @Static Array<String> GitTag.listMatch(String pattern, GitRepository repo);

/* Lookup a tag object from the repository. */
@Native @Static GitTag GitTag.lookup(GitRepository repo, GitOid id);

/* Lookup a tag object from the repository, given a prefix of its identifier
 * (short id). */
@Native @Static GitTag GitTag.lookupPrefix(GitRepository repo, GitOid id, int len);

/* Get the message of a tag */
@Native String GitTag.message();

/* Get the name of a tag */
@Native String GitTag.name();

/* Get the tagger (author) of a tag */
@Native GitSignature GitTag.tagger();

/* Get the tagged object of a tag */
@Native GitObject GitTag.target();

/* Get the OID of the tagged object of a tag */
@Native GitOid GitTag.targetOid();

/* Get the type of a tag's tagged object */
@Native int GitTag.type();

/* ------------------------------------------------------------------------ */
// [transport]

/* Get the appropriate transport for an URL. */
@Native GitTransport GitTransport.new(String url);

/* ------------------------------------------------------------------------ */
// [tree]

/* Close an open tree */
@Native void GitTree.close();

/* Write a tree to the ODB from the index file */
@Native @Static GitOid GitTree.createFromIndex(GitIndex index);

/* Get the UNIX file attributes of a tree entry */
@Native int GitTreeEntry.attributes();

/* Lookup a tree entry by its position in the tree */
@Native @Static GitTreeEntry GitTreeEntry.byIndex(GitTree tree, int idx);

/* Lookup a tree entry by its filename */
@Native @Static GitTreeEntry GitTreeEntry.byName(GitTree tree, String filename);
/* Get the id of the object pointed by the entry */
@Native GitOid GitTreeEntry.id();

/* Get the filename of a tree entry */
@Native String GitTreeEntry.name();

/* Get the type of the object pointed by the entry */
@Native int GitTreeEntry.type();

/* Get the number of entries listed in a tree */
@Native int GitTree.entryCount();

/* Get the id of a tree. */
@Native GitOid GitTree.id();

/* Lookup a tree object from the repository. */
@Native @Static GitTree GitTree.lookup(GitRepository repo, GitOid id);

/* Lookup a tree object from the repository, given a prefix of its identifier
 * (short id). */
@Native @Static GitTree GitTree.lookupPrefix(GitRepository repo, GitOid id, int len);

/* ------------------------------------------------------------------------ */
// [treebuilder]

/* Clear all the entires in the builder */
@Native void GitTreebuilder.clear();

/* Create a new tree builder. */
@Native @Static GitTreebuilder GitTreebuilder.create(GitTree source);

/* Filter the entries in the tree */
@Native void GitTreebuilder.filter(Func<GitTreeEntry=>int> filter);

/* Free a tree builder */
@Native void GitTreebuilder.free();

/* Get an entry from the builder from its filename */
@Native GitTreeEntry GitTreebuilder.get(String filename);

/* Add or update an entry to the builder */
@Native GitTreeEntry GitTreebuilder.insert(String filename, GitOid id, int attributes);

/* Remove an entry from the builder by its filename */
@Native void GitTreebuilder.remove(String filename);

/* Write the contents of the tree builder as a tree object */
@Native GitOid GitTreebuilder.write(GitRepository repo);
