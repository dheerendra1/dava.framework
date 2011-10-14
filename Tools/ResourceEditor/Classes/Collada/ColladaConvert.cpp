#include "ColladaConvert.h"
#include "ColladaDocument.h"


using namespace DAVA;


void ConvertDaeToSce(const String & pathToFile)
{
    FCollada::Initialize();
    
    ColladaDocument colladaDocument;
    if (!colladaDocument.Open(pathToFile.c_str()))
    {
        printf("*** ERROR: Failed to read %s\n", pathToFile.c_str());
        return;
    }
    
    /*
     int paramCount = CommandLineParser::Instance()->GetParamCount();
     for (int k = PARAM_SOURCE_FILEPATH + 1; k < paramCount; ++k)
     {
     colladaDocument.ExportAnimations(CommandLineParser::Instance()->GetParam(k).c_str());
     }
     */
    String fileDirectory, filePath;
    FileSystem::SplitPath(pathToFile, fileDirectory, filePath);
    filePath = FileSystem::ReplaceExtension(filePath, ".sce");
    
    colladaDocument.SaveScene(fileDirectory, filePath);
    colladaDocument.Close();
    
    FCollada::Release();
}