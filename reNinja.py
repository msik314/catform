import sys
import os
import build.ninja_syntax as ns

shaderExt = \
{\
    "vs":"vert",\
    "tsc":"tesc",\
    "tse":"tese",\
    "gs":"geom",\
    "fs":"frag",\
    "ck":"comp",\
    "msh":"mesh",\
    "tks":"task",\
    "rgs":"rgen",\
    "ris":"rint",\
    "ras":"rahit",\
    "rcs":"rchit",\
    "rms":"rmiss",\
    "rfn":"rcall"\
}

def shaderBin(ninja, shader, outDir):
    ext = shader[shader.rfind(".") + 1:]
    return os.path.join(outDir, shader[0:shader.rfind(".")] + shaderExt[ext].title() + ".spv")

def buildShader(ninja, shader, inDir, outDir):
    ext = shader[shader.rfind(".") + 1:]
    ninja.build(shaderBin(ninja, shader, outDir), shaderExt[ext], os.path.join(inDir, shader))

def main(args):
    outName = None
    srcC = {}
    srcCXX = {}
    oDbg = {}
    oRel = {}
    names = []
    shaders = []
    res = []
    copyCmd = "rm -rf $out; cp -r $in $out"
    oDbgPath = os.path.normpath("obj/debug")
    oRelPath = os.path.normpath("obj/release")
    binDbgPath = os.path.normpath("bin/debug")
    binRelPath = os.path.normpath("bin/release")
    excludeDirs = []
    debugTmp = None
    releaseTMP = None
    builds = None
    
    if len(args) > 1:
        outName = args[1]
    elif sys.platform == "win32" or sys.platform == "cygwin":
        outName = "a.exe"
    elif sys.platform == "linux":
        outName = "a.out"
    
    if sys.platform == "win32":
        copyCmd = "cmd /q /c \"del /s /q " + os.path.join("$out", "") + " & for %i in ($in) do xcopy %i " + os.path.join("$out", "") + " /q /y\""
    
    #Read exclude dirs
    if os.path.exists(os.path.normpath("build/exclude.txt")):
        with open("build/exclude.txt") as exc:
            lines = exc.readlines()
            for line in lines:
                excludeDirs.append(os.path.normpath(line.strip()))
    
    #List files
    for root, dirNames, fileNames in os.walk(".", topdown = True):
        for name in fileNames:
            dirNames[:] = [d for d in dirNames if d not in excludeDirs] 
            filePath = os.path.join(root, name)[2:]
            
            if name[-2:] == ".c":
                names.append(name)
                srcC[name] = filePath
                oDbg[name] = os.path.join(oDbgPath, name[0:-2] + ".o")
                oRel[name] = os.path.join(oRelPath, name[0:-2] + ".o")
            
            elif name[-4:] == ".cpp":
                names.append(name)
                srcCXX[name] = filePath
                oDbg[name] = os.path.join(oDbgPath, name[0:-4] + ".o")
                oRel[name] = os.path.join(oRelPath, name[0:-4] + ".o")
            
            elif name[-4:] == ".cxx":
                names.append(name)
                srcCXX[name] = filePath
                oDbg[name] = os.path.join(oDbgPath, name[0:-4] + ".o")
                oRel[name] = os.path.join(oRelPath, name[0:-4] + ".o")
                
            elif name[-4:] == ".cc":
                names.append(name)
                srcCXX[name] = filePath
                oDbg[name] = os.path.join(oDbgPath, name[0:-3] + ".o")
                oRel[name] = os.path.join(oRelPath, name[0:-3] + ".o")

    #Resources
    for root, dirNames, fileNames in os.walk("res"):
        for name in fileNames:
            filePath = os.path.join(root, name)
            
            ext = filePath[filePath.rfind("."):]
            if ext == ".spv": continue
            
            res.append(filePath)
            
    #Shaders
    for root, dirNames, fileNames in os.walk("shaders"):
        for name in fileNames:
            filePath = os.path.join(root, name)[8:]
            shaders.append(filePath)
            
    
    #Load templates
    with open("build/debugBuild.txt", "r") as f:
        debugTmp = f.read()
    
    with open("build/releaseBuild.txt", "r") as f:
        releaseTmp = f.read()
    
    #Create ninja files
    with open("debug.ninja", "w") as dbg:
        dbg.write(debugTmp + "\n")
        ninja = ns.Writer(dbg)
        
        ninja.rule("copy", copyCmd)
        
        for n in names:
            if n in srcC.keys():
                ninja.build(oDbg[n], "c", srcC[n])
            
            elif n in srcCXX.keys():
                ninja.build(oDbg[n], "cxx", srcCXX[n])
        
        for s in shaders:
            buildShader(ninja, s, "shaders", "res")
        
        ninja.build(os.path.join(binDbgPath, outName), "link", list(oDbg.values()))
        ninja.build(os.path.join(binDbgPath, "res"), "copy", [shaderBin(ninja, s, "res") for s in shaders] + res)
        ninja.default([os.path.join(binDbgPath, outName), os.path.join(binDbgPath, "res")])
    
    with open("release.ninja", "w") as rel:
        rel.write(releaseTmp + "\n")
        ninja = ns.Writer(rel)
        
        ninja.rule("copy", copyCmd)
        
        for n in names:
            if n in srcC.keys():
                ninja.build(oRel[n], "c", srcC[n])
            
            elif n in srcCXX.keys():
                ninja.build(oRel[n], "cxx", srcCXX[n])
        
        for s in shaders:
            buildShader(ninja, s, "shaders", "res")
        
        ninja.build(os.path.join(binRelPath, outName), "link", list(oRel.values()))
        ninja.build(os.path.join(binRelPath, "res"), "copy", [shaderBin(ninja, s, "res") for s in shaders] + res)
        ninja.default([os.path.join(binRelPath, outName), os.path.join(binRelPath, "res")])

if __name__ == "__main__":
    main(sys.argv)
