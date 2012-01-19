using konoha.libgit2.*;

/* fetch */
void renamePackfile(String packname, GitIndexer idx)
{
	String oid = idx.hash().fmt();
	String path = packname.substring(0, packname.lastIndexOf("/")) + "pack-" + oid + ".pack";
	OUT << "Renaming pack to " << path;
}

void fetch(GitRepository repo, String[] args)
{
	GitConfig cfg = repo.config("", "");
	if (cfg == null) {
		OUT << "cfg is null" << EOL;
		return;
	}

	if (args.size < 1) {
		args = [""];
	}
	OUT << "Fetching " << args[0] << EOL;
	GitRemote remote = GitRemote.get(cfg, args[0]);
	if (remote == null) {
		OUT << "remote is null" << EOL;
		return;
	}

	remote.connect(GitDirectory.FETCH);
	remote.negotiate();
	String packname = remote.download();
	if (packname == null) {
		OUT << "packname is null" << EOL;
		return;
	}

	OUT << "The packname is " << packname << EOL;
	GitIndexer idx = new GitIndexer(packname);
	if (idx == null) {
		OUT << "idx is null" << EOL;
		return;
	}

	GitIndexerStats stats = idx.run();
	if (stats == null) {
		OUT << "stats is null" << EOL;
		return;
	}

	OUT << "Received " << stats.total << " objects" << EOL;
	idx.write();
	renamePackfile(packname, idx);

	remote.updateTips();
}

Map<String,Func<GitRepository,String[]>> commands = {
	"fetch": fetch
};

void main(String[] args)
{
	if (args.size < 1) {
		ERR << "usage: konoha " << $script.name << " <cmd> [repo]" << EOL;
		exit(1);
	}

	String cmd = args[0];
	if (commands[cmd] != null) {
		GitRepository repo = GitRepository.open(".git");
		if (args.size == 1) {
			args = [];
		} else {
			args = args[1 to];
		}
		commands[cmd](repo, args);
	} else {
		ERR << "Command not found: " << args[0] << EOL;
	}
}
