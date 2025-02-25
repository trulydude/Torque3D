//-----------------------------------------------------------------------------
// Copyright (c) 2012 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#ifndef _SHADERGEN_GLSL_H_
#define _SHADERGEN_GLSL_H_

#ifndef _SHADERGEN_H_
#include "shaderGen/shaderGen.h"
#endif


class ShaderGenPrinterGLSL : public ShaderGenPrinter
{
        bool extraRTs[3];
 
public:
        ShaderGenPrinterGLSL() { for (int i = 0; i < 3; i++) extraRTs[i] = false; }
 
   // ShaderGenPrinter
   void printShaderHeader(Stream& stream) override;
   void printMainComment(Stream& stream) override;
   void printVertexShaderCloser(Stream& stream) override;
   void printPixelShaderOutputStruct(Stream& stream, const MaterialFeatureData &featureData) override;
   void printPixelShaderCloser(Stream& stream) override;
   void printLine(Stream& stream, const String& line) override;
};

class ShaderGenComponentFactoryGLSL : public ShaderGenComponentFactory
{
public:

   /// Helper function for converting a vertex decl 
   /// type to an GLSL type string.
   static const char* typeToString( GFXDeclType type );

   // ShaderGenComponentFactory
   ShaderComponent* createVertexInputConnector( const GFXVertexFormat &vertexFormat ) override;
   ShaderComponent* createVertexPixelConnector() override;
   ShaderComponent* createVertexParamsDef() override;
   ShaderComponent* createPixelParamsDef() override;
};

#endif