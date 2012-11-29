/*
 * BatteryTech
 * Copyright (c) 2010 Battery Powered Games LLC.
 *
 * This code is a component of BatteryTech and is subject to the 'BatteryTech
 * End User License Agreement'.  Among other important provisions, this
 * license prohibits the distribution of source code to anyone other than
 * authorized parties.  If you have any questions or would like an additional
 * copy of the license, please contact: support@batterypoweredgames.com
 */

//============================================================================
// Name        : TextRasterRenderer.h
// Description : Renders text from a font-baked texture
//============================================================================

#ifndef TEXTRASTERRENDERER_H_
#define TEXTRASTERRENDERER_H_

#include "Renderer.h"
#include "../decoders/stb_truetype.h"
#include "../platform/platformgl.h"
#include "GraphicsConfiguration.h"
#include "QuadRenderer.h"

namespace BatteryTech {

	class Context;
	class ShaderProgram;

	class TextRasterRenderer: public Renderer {
	public:
		TextRasterRenderer(Context *context, const char *assetName, F32 fontSize);
		/**
		 * Initializes the TextRasterRenderer for the GL context
		 */
		virtual void init(BOOL32 newContext);
		/**
		 * Renders a line of text using reference coordinate system.
		 * x = position relative to REFERENCE_WIDTH
		 * y = position relative to REFERENCE_HEIGHT
		 * Call startText() before any renders and finishText() after to reset GL states
		 */
		void render(const char *text, F32 x, F32 y, F32 scale = 1.0f);
		/**
		 * Renders a block of text using reference coordinate system.
		 * x = position relative to REFERENCE_WIDTH
		 * y = position relative to REFERENCE_HEIGHT
		 * Call startText() before any renders and finishText() after to reset GL states
		 */
		void renderMultiline(const char *text, F32 x, F32 y, F32 maxX, F32 maxY, F32 scale = 1.0f);

		/**
		 * Starts rendering text
		 * Must be called before first render()
		 */
		void startText();
		/**
		 * Finishes rendering text
		 * Must be called after render() calls are all complete
		 */
		void finishText();
		/**
		 * Gets the height of the text in reference-size units
		 */
		F32 getHeight(F32 scale = 1.0f);
		/**
		 * Measures the width of a line of text in reference-size units
		 */
		F32 measureWidth(const char *text, F32 scale = 1.0f);
		/**
		 * Calculates total height to be used for multiline text
		 * x = position relative to REFERENCE_WIDTH
		 * y = position relative to REFERENCE_HEIGHT
		 */
		F32 measureMultilineHeight(const char *text, F32 availableWidth, F32 scale = 1.0f);

		void setStroke(S32 inner, S32 outer);

		void setColorFilter(Vector4f rgba);

		virtual ~TextRasterRenderer();
	private:
        // All
		void applyInnerStroke(unsigned char* bitmap8, unsigned char* bitmap, S32 width, S32 height);
		void applyOuterStroke(unsigned char* bitmap8, unsigned char* bitmap, S32 width, S32 height);
        void applyAA(unsigned char* bitmap, S32 width, S32 height);
        void addToBuffer(Vector3f p0, Vector3f p1, Vector3f p2, Vector3f p3, Vector2f uv0, Vector2f uv1, Vector2f uv2, Vector2f uv3);
        void renderBuffer();
		Context *context;
		GLuint ftex;
		char *assetName;
		F32 fontSize;
		S32 innerStroke, outerStroke;
		Vector4f color;
		F32 vertSpaceMult;
        GLuint vertVBOId;
        GLuint idxVBOId;
        GLQuadVertex *vertBuffer;
        U16 *idxBuffer;
        BOOL32 isTTF;
        S32 bufferUsed;
        
        // TTF
        void loadTTF();
        void renderTTF(const char *text, F32 x, F32 y, F32 scale = 1.0f);
		void renderMultilineTTF(const char *text, F32 x, F32 y, F32 maxX, F32 maxY, F32 scale = 1.0f);
        F32 getTTFHeight(F32 scale = 1.0f);
        F32 measureTTFWidth(const char *text, F32 scale = 1.0f);
		F32 measureTTFMultilineHeight(const char *text, F32 availableWidth, F32 scale = 1.0f);
		stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
		S32 bmpWidth, bmpHeight;
        
        // BMFont
        void loadBMFont();
        void renderBMFont(const char *text, F32 x, F32 y, F32 scale = 1.0f);
		void renderMultilineBMFont(const char *text, F32 x, F32 y, F32 maxX, F32 maxY, F32 scale = 1.0f);
        F32 getBMFontHeight(F32 scale = 1.0f);
        F32 measureBMFontWidth(const char *text, F32 scale = 1.0f);
		F32 measureBMFontMultilineHeight(const char *text, F32 availableWidth, F32 scale = 1.0f);
        
        struct BMFontInfo {
            char *face;
            F32 size;
            BOOL32 bold;
            BOOL32 italic;
            char *charset;
            BOOL32 unicode;
            S32 stretchH;
            BOOL32 smooth;
            BOOL32 aa;
            Vector4f padding;
            Vector2f spacing;
            F32 outline;
            F32 lineHeight;
            F32 base;
            S32 scaleW, scaleH;
            S32 pages;
            BOOL32 packed;
            BYTE8 alphaChnl, redChnl, greenChnl, blueChnl;
            BMFontInfo() {
                face = NULL;
                charset = NULL;
                size = 0;
                bold = italic = unicode = smooth = aa = FALSE;
                stretchH = 0;
                padding = Vector4f(0,0,0,0);
                spacing = Vector2f(0,0);
                outline = 0;
                lineHeight = 0;
                base = 0;
                scaleW = scaleH = 0;
                pages = 0;
                packed = FALSE;
                alphaChnl = redChnl = greenChnl = blueChnl = 0;
            }
            virtual ~BMFontInfo() {
                delete [] face;
                face = NULL;
                delete [] charset;
                charset = NULL;
            }
        };
        
        struct BMFontPage {
            S32 id;
            char *assetName;
            BMFontPage() {
                id = 0;
                assetName = NULL;
            }
            virtual ~BMFontPage() {
                delete [] assetName;
                assetName = NULL;
            }
        };
        
        struct BMFontChar {
            S32 id;
            S32 x,y;
            S32 width, height;
            S32 xoffset, yoffset;
            S32 xadvance;
            S32 page;
            S32 chnl;
            Vector2f uvs[4];
            BMFontChar() {
                id = 0;
                x = y = 0;
                width = height = 0;
                xoffset = yoffset = 0;
                xadvance = 0;
                page = 0;
                chnl = 0;
            }
        };
        
        BMFontInfo *bmInfo;
        BMFontPage *bmFontPage;
        HashTable<S32, BMFontPage*> *bmFontPageTable;
        HashTable<S32, BMFontChar*> *bmFontCharTable;
        HashTable<S32, HashTable<S32, F32>*> *bmKerningTable;
        S32 bmCurrentPage;
	};

}
#endif /* TEXTRASTERRENDERER_H_ */
