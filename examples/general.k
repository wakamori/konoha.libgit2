using konoha.libgit2.*;

GIT_DIR="/path/to/.git";
GIT_CONFIG_DIR="/path/to/.gitconfig";

/* Opening the Repository */
GitRepository repo = GitRepository.open(GIT_DIR);

/* SHA-1 Value Conversions */
String hex = "fd6e612585290339ea8bf39c692a7ff6a29cb7c3";
OUT << "*Hex to Raw*" << EOL;
GitOid oid = GitOid.fromStr(hex);
OUT << "Raw 20 bytes: " << oid.id << EOL << EOL;
OUT << "*Raw to Hex*" << EOL;
String out = oid.fmt();
OUT << "SHA hex string: " << out << EOL << EOL;

/* Working with the Object Database */
GitOdb odb = repo.database();

/* Raw Object Reading */
OUT << "*Raw Object Read*" << EOL;
GitOdbObject obj = odb.read(oid);
Bytes data = obj.data();
int type = obj.type();

OUT << "object length and type: " << obj.size() << ", " << type << EOL;

obj.close();

/* Raw Object Writing */
OUT << "*Raw Object Write*" << EOL;
oid = odb.write((to Bytes)"test data", GitObject.BLOB);
out = oid.fmt();
OUT << "Written Object: " << out << EOL << EOL;

odb.close();

/* Object Parsing */

/* Commit Parsing */
OUT << "*Commit Parsing*" << EOL;
oid = GitOid.fromStr("f0877d0b841d75172ec404fc9370173dfffc20d1");
GitCommit commit = GitCommit.lookup(repo, oid);
String message = commit.message();
GitSignature author = commit.author();
GitSignature cmtter = commit.committer();
int ctime = commit.time();

OUT << "Author: " << author.name << " (" << author.email << ")" << EOL;

int parents = commit.parentCount();
for (int p = 0; p < parents; p++) {
	GitCommit parent = commit.parent(p);
	out = parent.id().fmt();
	OUT << "Parent: " << out << EOL;
	parent.close();
}

/* Writing Commits */
OUT << EOL << "*Commit Writing*" << EOL;
author = new GitSignature("Scott Chacon", "schacon@gmail.com", 123456789, 60);
cmtter = new GitSignature("Scott A Chacon", "scott@github.com", 987654321, 90);

GitOid tree_id = GitOid.fromStr("28873d96b4e8f4e33ea30f4c682fd325f7ba56ac");
GitTree tree = GitTree.lookup(repo, tree_id);
GitOid parent_id = GitOid.fromStr("f0877d0b841d75172ec404fc9370173dfffc20d1");
GitCommit parent = GitCommit.lookup(repo, parent_id);

GitOid commit_id = GitCommit.create(repo, null, author, cmtter, null,
		"example commit", tree, [parent]);

author.free();
cmtter.free();

out = commit_id.fmt();
OUT << "New Commit: " << out << EOL << EOL;

/* Tag Parsing */
OUT << "*Tag Parsing*" << EOL;
oid = GitOid.fromStr("bc422d45275aca289c51d79830b45cecebff7c3a");
GitTag tag = GitTag.lookup(repo, oid);
GitObject commit_obj = tag.target();
String tname = tag.name();
int ttype = tag.type();
String tmessage = tag.message();
OUT << "Tag Message: " << tmessage << EOL << EOL;

commit_obj.close();

/* Tree Parsing */
OUT << "*Tree Parsing*" << EOL;

oid = GitOid.fromStr("2a741c18ac5ff082a7caaec6e74db3075a1906b5");
tree = GitTree.lookup(repo, oid);

int cnt = tree.entryCount(); // 3
OUT << "tree entries: " << cnt << EOL;
GitTreeEntry entry = GitTreeEntry.byIndex(tree, 0);
OUT << "Entry name: " << entry.name() << EOL << EOL;
//entry = GitTreeEntry.byName(tree, "hello.c");
//entry.name(); // "hello.c"

/* Blob Parsing */
OUT << "*Blob Parsing*" << EOL;
oid = GitOid.fromStr("af7574ea73f7b166f869ef1a39be126d9a186ae0");
GitBlob blob = GitBlob.lookup(repo, oid);

OUT << "Blob Size: " << blob.rawSize() << EOL << EOL;
//blob.rawContent(); // "content"

/* Revwalking */
OUT << "*Revwalking*" << EOL;
oid = GitOid.fromStr("f0877d0b841d75172ec404fc9370173dfffc20d1");
GitRevwalk walk = new GitRevwalk(repo);
walk.sorting(GitSort.TOPOLOGICAL | GitSort.REVERSE);
walk.push(oid);

while ((oid = walk.next()) != null) {
	GitCommit wcommit GitCommit.lookup(repo, oid);
	String cmsg = wcommit.message();
	GitSignature cauth = wcommit.author();
	OUT << cmsg << " (" << cauth.email << ")" << EOL;
	cauth.free();
}

walk.free();

/* Index File Manipulation */
OUT << EOL << "*Index Walking*" << EOL;

GitIndex index = repo.index();
int ecount = index.entryCount();
print ecount;
for (int i = 0; i < ecount; i++) {
	GitIndexEntry e = index.get(i);
	OUT << "path: " << e.path << EOL;
	OUT << "mtime: " << e.mtime << EOL;
	OUT << "fs: " << e.fileSize << EOL;
}

index.free();

/* References */
OUT << EOL << "*Reference Listing*" << EOL;

String[] ref_list = GitReference.listAll(repo, GitReference.LISTALL);

for (int i = 0; i < ref_list.size; i++) {
	String refname = ref_list[i];
	GitReference ref = GitReference.lookup(repo, refname);
	switch (ref.type()) {
	case GitReference.OID:
		out = ref.oid().fmt();
		OUT << refname << " [" << out << "]" << EOL;
		break;
	case GitReference.SYMBOLIC:
		OUT << refname << " => " << ref.target() << EOL;
		break;
	default:
		ERR << "Unexpected reference type" << EOL;
		exit(1);
	}
}

/* Config files */
OUT << EOL << "*Config Listing*" << EOL;

GitConfig cfg = GitConfig.openOndisk(GIT_CONFIG_DIR);
boolean j = cfg.getBool("help.autocorrect");
OUT << "Autocorrect: " << j << EOL;
String email = cfg.getString("user.email");
OUT << "Email: " << email << EOL;
repo.free();
