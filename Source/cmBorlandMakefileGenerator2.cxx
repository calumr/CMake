/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile$
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

Copyright (c) 2001 Insight Consortium
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * The name of the Insight Consortium, nor the names of any consortium members,
   nor of any contributors, may be used to endorse or promote products derived
   from this software without specific prior written permission.

  * Modified source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
#include "cmBorlandMakefileGenerator2.h"
#include "cmMakefile.h"
#include "cmStandardIncludes.h"
#include "cmSystemTools.h"
#include "cmSourceFile.h"
#include "cmMakeDepend.h"
#include "cmCacheManager.h"
#include "cmGeneratedFileStream.h"
#include "windows.h"


cmBorlandMakefileGenerator2::cmBorlandMakefileGenerator2()
{
  this->SetLibraryPathOption("-L");
  this->SetLibraryLinkOption("");
}

cmBorlandMakefileGenerator2::~cmBorlandMakefileGenerator2()
{
}


void cmBorlandMakefileGenerator2::ComputeSystemInfo()
{
  // now load the settings
  if(!m_Makefile->GetDefinition("CMAKE_ROOT"))
    {
    cmSystemTools::Error(
      "CMAKE_ROOT has not been defined, bad GUI or driver program");
    return;
    }
  std::string fpath = 
    m_Makefile->GetDefinition("CMAKE_ROOT");
  fpath += "/Templates/CMakeWindowsBorlandConfig2.cmake";
  m_Makefile->ReadListFile(NULL,fpath.c_str());
}


  
void cmBorlandMakefileGenerator2::OutputMakeVariables(std::ostream& fout)
{
  fout << "# NMake Makefile generated by cmake\n";
  const char* variables = 
    "# general varibles used in the makefile\n"
    "\n"
    "CMAKE_STANDARD_WINDOWS_LIBRARIES = @CMAKE_STANDARD_WINDOWS_LIBRARIES@\n"
    "CMAKE_C_FLAGS        = @CMAKE_C_FLAGS@ @BUILD_FLAGS@\n"
    "CMAKE_OBJECT_FILE_SUFFIX            = @CMAKE_OBJECT_FILE_SUFFIX@\n"
    "CMAKE_EXECUTABLE_SUFFIX             = @CMAKE_EXECUTABLE_SUFFIX@\n"
    "CMAKE_STATICLIB_SUFFIX              = @CMAKE_STATICLIB_SUFFIX@\n"
    "CMAKE_SHLIB_SUFFIX                  = @CMAKE_SHLIB_SUFFIX@\n"
    "CMAKE_CXX_FLAGS      = -P @CMAKE_CXX_FLAGS@ @BUILD_FLAGS@\n";
  std::string buildType = "CMAKE_CXX_FLAGS_";
  buildType +=  m_Makefile->GetDefinition("CMAKE_BUILD_TYPE");
  buildType = cmSystemTools::UpperCase(buildType);
  m_Makefile->AddDefinition("BUILD_FLAGS",
                            m_Makefile->GetDefinition(
                              buildType.c_str()));
  std::string replaceVars = variables;
  m_Makefile->ExpandVariablesInString(replaceVars);
  
  std::string ccompiler = m_Makefile->GetDefinition("CMAKE_C_COMPILER");
  cmSystemTools::ConvertToWindowsSlashes(ccompiler);
  fout << "CMAKE_C_COMPILER  = " << cmSystemTools::EscapeSpaces(ccompiler.c_str())
       << "\n";
  std::string cxxcompiler = m_Makefile->GetDefinition("CMAKE_CXX_COMPILER");
  cmSystemTools::ConvertToWindowsSlashes(cxxcompiler);
  fout << "CMAKE_CXX_COMPILER  = " << cmSystemTools::EscapeSpaces(cxxcompiler.c_str())
       << "\n";

  
  std::string cmakecommand = m_Makefile->GetDefinition("CMAKE_COMMAND");
  cmSystemTools::ConvertToWindowsSlashes(cmakecommand);
  fout << "CMAKE_COMMAND = " << cmSystemTools::EscapeSpaces(cmakecommand.c_str()) << "\n";

  fout << replaceVars.c_str();
  fout << "CMAKE_CURRENT_SOURCE = " 
       << ShortPath(m_Makefile->GetStartDirectory() )
       << "\n";
  fout << "CMAKE_CURRENT_BINARY = " 
       << ShortPath(m_Makefile->GetStartOutputDirectory())
       << "\n";
  fout << "CMAKE_SOURCE_DIR = " 
       << ShortPath(m_Makefile->GetHomeDirectory()) << "\n";
  fout << "CMAKE_BINARY_DIR = " 
       << ShortPath(m_Makefile->GetHomeOutputDirectory() )
       << "\n";
  // Output Include paths
  fout << "INCLUDE_FLAGS = ";
  std::vector<std::string>& includes = m_Makefile->GetIncludeDirectories();
  std::vector<std::string>::iterator i;
  fout << "-I" << cmSystemTools::EscapeSpaces(m_Makefile->GetStartDirectory()) << " ";
  for(i = includes.begin(); i != includes.end(); ++i)
    {
    std::string include = *i;
    // Don't output a -I for the standard include path "/usr/include".
    // This can cause problems with certain standard library
    // implementations because the wrong headers may be found first.
    if(include != "/usr/include")
      {
      fout << "-I" << cmSystemTools::EscapeSpaces(i->c_str()).c_str() << " ";
      }
    } 
  fout << m_Makefile->GetDefineFlags();
  fout << "\n\n";
}


void cmBorlandMakefileGenerator2::BuildInSubDirectory(std::ostream& fout,
                                                  const char* directory,
                                                  const char* target1,
                                                  const char* target2)
{
  if(target1)
    {
    std::string dir = directory;
    cmSystemTools::ConvertToWindowsSlashes(dir);
    fout << "\tif not exist " << cmSystemTools::EscapeSpaces(dir.c_str())
         << " " 
         << "$(MAKE) rebuild_cache\n"
         << "\tcd .\\" << directory << "\n"
         << "\t$(MAKE) -$(MAKEFLAGS) " << target1 << "\n";
    }
  if(target2)
    {
    fout << "\t$(MAKE) -$(MAKEFLAGS) " << target2 << "\n";
    }
  std::string currentDir = m_Makefile->GetCurrentOutputDirectory();
  cmSystemTools::ConvertToWindowsSlashes(currentDir);
  fout << "\tcd " << cmSystemTools::EscapeSpaces(currentDir.c_str()) << "\n";
}




// This needs to be overriden because nmake requires commands to be quoted
// if the are full paths to the executable????

void cmBorlandMakefileGenerator2::OutputMakeRule(std::ostream& fout, 
                                              const char* comment,
                                              const char* target,
                                              const char* depends, 
                                              const char* command,
                                              const char* command2,
                                              const char* command3,
                                              const char* command4)
{
  if(!target)
    {
    cmSystemTools::Error("no target for OutputMakeRule");
    return;
    }
  
  std::string replace;
  if(comment)
    {
    replace = comment;
    m_Makefile->ExpandVariablesInString(replace);
    fout << "#---------------------------------------------------------\n";
    fout << "# " << comment;
    fout << "\n#\n";
    }
  fout << "\n";
  replace = target;
  m_Makefile->ExpandVariablesInString(replace);
  replace = cmSystemTools::EscapeSpaces(replace.c_str());
  fout << replace.c_str() << ": ";
  if(depends)
    {
    replace = depends;
    m_Makefile->ExpandVariablesInString(replace);
    fout << replace.c_str();
    }
  fout << "\n";
  const char* startCommand = "\t";
  const char* endCommand = "\n";
  if(command)
    {
    replace = ShortPathCommand(command);
    m_Makefile->ExpandVariablesInString(replace);
    fout << startCommand << replace.c_str() << endCommand;
    }
  if(command2)
    {
    replace = ShortPathCommand(command2);
    m_Makefile->ExpandVariablesInString(replace);
    fout << startCommand << replace.c_str() << endCommand;
    }
  if(command3)
    {
    replace = ShortPathCommand(command3);
    m_Makefile->ExpandVariablesInString(replace);
    fout << startCommand << replace.c_str() << endCommand;
    }
  if(command4)
    {
    replace = ShortPathCommand(command4);
    m_Makefile->ExpandVariablesInString(replace);
    fout << startCommand << replace.c_str() << endCommand;
    }
  fout << "\n";
  // reset m_QuoteNextCommand, as the default should be to quote the 
  // commands.   We need the quotes when the command has a full path
  // to an executable.  However, the quotes break things like the
  // linker command.
  m_QuoteNextCommand = true;
}

void 
cmBorlandMakefileGenerator2::
OutputBuildObjectFromSource(std::ostream& fout,
                            const char* shortName,
                            const cmSourceFile& source,
                            const char* extraCompileFlags,
                            bool shared)
{ 
  std::string comment = "Build ";
  std::string objectFile = std::string(shortName) + 
    this->GetOutputExtension(source.GetSourceExtension().c_str());
  cmSystemTools::ConvertToWindowsSlashes(objectFile);
  comment += objectFile + "  From ";
  comment += source.GetFullPath();
  std::string compileCommand;
  std::string ext = source.GetSourceExtension();
  if(ext == "c" )
    {
    compileCommand = "$(CMAKE_C_COMPILER) $(CMAKE_C_FLAGS) ";
    compileCommand += extraCompileFlags;
    if(shared)
      {
      compileCommand += "$(CMAKE_SHLIB_CFLAGS) ";
      }
    compileCommand += " -o";
    compileCommand += objectFile;
    compileCommand += " $(INCLUDE_FLAGS) -c ";
    compileCommand += 
      cmSystemTools::EscapeSpaces(source.GetFullPath().c_str());
    }
  else if (ext == "rc")
    {
    compileCommand = "$(RC) -o\"";
    compileCommand += objectFile;
    compileCommand += "\" ";
    compileCommand += 
      cmSystemTools::EscapeSpaces(source.GetFullPath().c_str());
    }
  else if (ext == "def")
    {
    // no rule to output for this one
    return;
    }
  // assume c++ if not c rc or def
  else
    {
    compileCommand = "$(CMAKE_CXX_COMPILER) $(CMAKE_CXX_FLAGS) ";
    compileCommand += extraCompileFlags;
    if(shared)
      {
      compileCommand += "$(CMAKE_SHLIB_CFLAGS) ";
      }
    compileCommand += " -o";
    compileCommand += objectFile;
    compileCommand += " $(INCLUDE_FLAGS) -c ";
    compileCommand += 
      cmSystemTools::EscapeSpaces(source.GetFullPath().c_str());
    }
  m_QuoteNextCommand = false;
  this->OutputMakeRule(fout,
                       comment.c_str(),
                       objectFile.c_str(),
                       cmSystemTools::EscapeSpaces(
                         source.GetFullPath().c_str()).c_str(),
                       compileCommand.c_str());
}

void cmBorlandMakefileGenerator2::OutputSharedLibraryRule(std::ostream& fout, 
                                                       const char* name,
                                                       const cmTarget &t)
{
  std::string target = m_LibraryOutputPath + name;
  std::string libpath = target + ".lib";
  target += ".dll";
  cmSystemTools::ConvertToWindowsSlashes(libpath);
  cmSystemTools::ConvertToWindowsSlashes(target);
  target = cmSystemTools::EscapeSpaces(target.c_str());
  libpath = cmSystemTools::EscapeSpaces(libpath.c_str());
  std::string depend = "$(";
  depend += name;
  depend += "_SRC_OBJS) $(" + std::string(name) + "_DEPEND_LIBS)";
  std::string command = "$(CMAKE_CXX_COMPILER) -tWD  @&&|\n";
  // must be executable name
  command += "-e";
  command += target;
  command += " ";
  std::strstream linklibs;
  this->OutputLinkLibraries(linklibs, name, t);
  linklibs << std::ends;
  // then the linker options -L and libraries (any other order will fail!)
  command += linklibs.str();
  delete [] linklibs.str();
  // then list of object files
  command += " $(" + std::string(name) + "_SRC_OBJS) ";
  std::string command2 = "implib -w ";
  command2 += libpath + " " + target;
  const std::vector<cmSourceFile>& sources = t.GetSourceFiles();
  for(std::vector<cmSourceFile>::const_iterator i = sources.begin();
      i != sources.end(); ++i)
    {
    if(i->GetSourceExtension() == "def")
      {
      command += "";
      command += i->GetFullPath();
      }
    }
  command += "\n|\n";
  m_QuoteNextCommand = false;
    
  this->OutputMakeRule(fout, "rules for a shared library",
                       target.c_str(),
                       depend.c_str(),
                       command.c_str(),
                       command2.c_str());
}

void cmBorlandMakefileGenerator2::OutputModuleLibraryRule(std::ostream& fout, 
                                                       const char* name, 
                                                       const cmTarget &target)
{
  this->OutputSharedLibraryRule(fout, name, target);
}

void cmBorlandMakefileGenerator2::OutputStaticLibraryRule(std::ostream& fout, 
                                                       const char* name,
                                                       const cmTarget &)
{
  std::string target = m_LibraryOutputPath + std::string(name) + ".lib";
  cmSystemTools::ConvertToWindowsSlashes(target);
  target = cmSystemTools::EscapeSpaces(target.c_str());
  std::string depend = "$(";
  depend += std::string(name) + "_SRC_OBJS)";
  std::string command = "tlib  @&&|\n\t /u ";
  command += target;
  std::string deleteCommand = "if exist ";
  deleteCommand +=  target;
  deleteCommand += " del ";
  deleteCommand += target;
  command += " $(";
  command += std::string(name) + "_SRC_OBJS)";
  command += "\n|\n";
  std::string comment = "rule to build static library: ";
  comment += name;
  m_QuoteNextCommand = false;
  this->OutputMakeRule(fout,
                       comment.c_str(),
                       target.c_str(),
                       depend.c_str(),
                       deleteCommand.c_str(),
                       command.c_str());
}

void cmBorlandMakefileGenerator2::OutputExecutableRule(std::ostream& fout,
                                                    const char* name,
                                                    const cmTarget &t)
{
  std::string target = m_ExecutableOutputPath + name + m_ExecutableExtension;
  cmSystemTools::ConvertToWindowsSlashes(target);
  target = cmSystemTools::EscapeSpaces(target.c_str());
  std::string depend = "$(";
  depend += std::string(name) + "_SRC_OBJS) $(" + std::string(name) + "_DEPEND_LIBS)";
  std::string command = 
    "$(CMAKE_CXX_COMPILER) ";
  command += " -e" + target;
  if(t.GetType() == cmTarget::WIN32_EXECUTABLE)
    {
    command +=  " -tWM ";
    }
  else
    {
    command += " -tWC ";
    }
  std::strstream linklibs;
  this->OutputLinkLibraries(linklibs, 0, t);
  linklibs << std::ends;
  command += linklibs.str();
  delete [] linklibs.str();
  command += " $(" + std::string(name) + "_SRC_OBJS) ";
  
  std::string comment = "rule to build executable: ";
  comment += name;
  m_QuoteNextCommand = false;
  this->OutputMakeRule(fout, 
                       comment.c_str(),
                       target.c_str(),
                       depend.c_str(),
                       command.c_str());
}

  


std::string cmBorlandMakefileGenerator2::GetOutputExtension(const char* s)
{
  std::string sourceExtension = s;
  if(sourceExtension == "def")
    {
    return "";
    }
  if(sourceExtension == "ico" || sourceExtension == "rc2")
    {
    return "";
    }
  if(sourceExtension == "rc")
    {
    return ".res";
    }
  return ".obj";
}



bool cmBorlandMakefileGenerator2::SamePath(const char* path1, const char* path2)
{
  // first check to see if they are the same anyway
  if (strcmp(path1, path2) == 0)
    {
    return true;
    }
  // next short path and lower case both of them for the compare
  return 
    cmSystemTools::LowerCase(ShortPath(path1)) ==
    cmSystemTools::LowerCase(ShortPath(path2));
}

void cmBorlandMakefileGenerator2::OutputBuildLibraryInDir(std::ostream& fout,
						       const char* path,
						       const char* s,
						       const char* fullpath)
{
  cmNMakeMakefileGenerator::OutputBuildLibraryInDir(fout, path, s, fullpath);
}


std::string cmBorlandMakefileGenerator2::ConvertToNativePath(const char* s)
{
  std::string ret = s;
  cmSystemTools::ConvertToWindowsSlashes(ret);
  return ret;
}

