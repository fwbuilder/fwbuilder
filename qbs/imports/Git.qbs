import qbs 1.0
import qbs.Process

Probe {
    property string version
    property string generation

    configure: {
        revision = undefined
        var p = new Process()
        if (p.exec("git", ["describe", "--tags", "--always"]) === 0) {
            version = p.readStdOut().trim().slice(1);
            generation = version.slice(0, version.lastIndexOf('.'))
        } else {
            console.error("Git not found")
        }
    }
}
