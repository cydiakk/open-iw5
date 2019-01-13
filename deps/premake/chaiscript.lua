chaiscript = {
	source = path.join(dependencies.basePath, "ChaiScript"),
}

function chaiscript.import()
	chaiscript.includes()
end

function chaiscript.includes()
	includedirs {
		path.join(chaiscript.source, "include"),
	}
end

function chaiscript.project()

end

table.insert(dependencies, chaiscript)
