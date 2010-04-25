/*

Copyright (c) 2010, Jonathan Wayne Parrott

Please see the license.txt file included with this source
distribution for more information.

*/

#ifndef __PHOENIXRS__
#define __PHOENIXRS__

#include <string>
#include <boost/bind.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/noncopyable.hpp>
#include "config.h"
#include "ResourceManager.h"
#include "Timer.h"
#include "Vector2d.h"
#include "Rectangle.h"
#include "Polygon.h"
#include "WindowManager.h"
#include "BatchRenderer.h"
#include "AbstractGeometryFactory.h"
#include "DebugConsole.h"
#include "2dGraphicsFactory.h"
#include "Font.h"

//! The phoenix namespace.
namespace phoenix
{

    //! The render system.
    /*!
        This is the heart of the Core. It ties together a ResourceManager, a BatchRender, Timer, and Fonts
        to provide a simply interface to drawing 2d objects in OpenGL. It provides texture loading and management,
        geometry factories and management, font loading and management, and access to a global resource manager.
        It also provides one entry point to initialize all of phoenix's subsystems.
    */
    class RenderSystem
		: public boost::noncopyable
    {

    public:


        //! Constructor
        /*!
           Creates a render system creates a window and opengl context. Once the system is constructed
           you are ready to start drawing stuff. It also initializes the singleton WindowManager..
           \sa run()
           \param _sz The size of the screen (default 640,480).
           \param _fs Full screen (default false).
        */
		RenderSystem( const Vector2d& _sz = Vector2d(640,480),bool _fs = false )
			: renderer(), 
			factory( renderer ),
			console(),
			resize(false), 
			fpstimer(), 
			framerate(1.0f), 
			font(0), 
			quit(false), 
			event_connection(),
			resources()
		{
			initialize( _sz, _fs );
		}

		//! Initialize
		/*!
			(Re)-initialize a rendersystem. This is automatically called by the constructor
			but it may be desirable sometimes to re-initialize a render system.
			\param _reint True if reinitializing an existing system ( usually true ).
			\note Throws and std::exception if problems arrive.
			\note Re-initialization invalides all OpenGL textures, etc. You should release all handles to resources, the debug console, everything before calling this.
		*/
		void initialize( const Vector2d& _sz = Vector2d(640,480), bool _fs = false, bool _reint = true  );

        //! Destruct.
		/*!
			Releases all resource. Also called WindowManager::close().
		*/
        ~RenderSystem();

        //! Enable screen resizing ( disabled by default ).
        inline void enableResize() { resize = true; }

        //! Disable screen resizing
        inline void disableResize() { resize = false; }

        //! Set blend mode.
        /*!
            Sets the blend mode for opengl. The default blend mode is GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA.
            If this is called without arguments, it restores the default blendmode.
            \param src The source factor, can be GL_ZERO, GL_ONE,
            GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR, GL_SRC_ALPHA,
            GL_ONE_MINUS_SRC_ALPHA, GL_DST_ALPHA,
            GL_ONE_MINUS_DST_ALPHA, and GL_SRC_ALPHA_SATURATE
            \param dst The destination factor, can be GL_ZERO, GL_ONE,
            GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR, GL_SRC_ALPHA,
            GL_ONE_MINUS_SRC_ALPHA, GL_DST_ALPHA, and
            GL_ONE_MINUS_DST_ALPHA
        */
        inline static void setBlendMode(const GLenum& src = GL_SRC_ALPHA, const GLenum& dst = GL_ONE_MINUS_SRC_ALPHA ) { glBlendFunc(src,dst); }

        //! Get the system's batch renderer.
        inline BatchRenderer& getBatchRenderer() { return renderer; }

        //! Get the system's 2d graphics factory.
        inline GraphicsFactory2d& getGraphicsFactory(){ return factory; }

        //! Get the system's internal resource manager.
        /*!
            You can add arbitrary resources to the system's resource manager. The system
            uses it to keep track of Textures and Fonts, but it is freely availible for the user
            to stick custom resources in.
        */
        inline ResourceManager& getResourceManager() { return resources; }

		//! Gets the system's debug console.
		inline DebugConsole& getDebugConsole() { return *console; }

        //! Set the system's current font for drawing text.
        inline void setFont( FontPtr t ) { font = t; }

        //! Get the current font used for drawing text.
        inline FontPtr getFont() const { return font; }

        //! Run.
        /*!
            Run takes care of swapping buffers, updating events, clearing the screen, and tracking time.
			It calls WindowManager::update() which in turns causes a WET_UPDATE event to be signaled.
        */
        bool run();

		//! Clears the screen to the given color.
        inline static void clearScreen( const Color& _c = Color(0,0,0) )
        {
            glClearColor( _c.getRed()/255.0f,_c.getGreen()/255.0f,_c.getBlue()/255.0f,_c.getAlpha()/255.0f );
            #ifdef DISABLE_DEPTHBUFFER
                glClear(GL_COLOR_BUFFER_BIT);
            #else
                glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
            #endif
        }

		//! Window Event Listener.
		/*!
			Listens for events from the WindowManager. If the user closes the window or presses
			the esc key, then run will return false.
		*/
		void onWindowEvent( const WindowEvent& e );

        //! Get frames per second.
        inline const double getFPS() const { return framerate; }

        //! Get time.
        /*!
            \return The amout in time ( in seconds ) that has occured since the last time run was called.
        */
        inline const double getTime() const { return fpstimer.getTime(); }

        //! Load texture.
        /*!
            Loads an image as a texture and adds it to the texture manager for garbage collection. 
			Can load .png, .tga, .bmp and .jpg or any other format supported by SOIL.
            \param _fn The filename of the image to load.
            \param _l Tells the loader to use linear filtering or not. (default true).
            \note Use nearest filtering for tilemaps, or anything that may look bad when scaled.
            \note Textures must be sizes that are a power of two. NPOT textures will experience artifacts (or may fail all together).
        */
        TexturePtr loadTexture( const std::string& _fn , bool _l = true);

		//! Load texture ( from memory )
        /*!
            Loads an image as a texture and adds it to the texture manager for garbage collection. This
			function loads the file from memory as if it was a regular file.
			Can load .png, .tga, .bmp and .jpg or any other format supported by SOIL.
            \param _d The data buffer.
			\param _len The length of the data buffer
			\param _w Width
			\param _h Height
            \param _lin Tells the loader to use linear filtering or not. (default true).
            \note Use nearest filtering for tilemaps, or anything that may look bad when scaled.
            \note Textures must be sizes that are a power of two. NPOT textures will experience artifacts (or may fail all together).
        */
		TexturePtr loadTexture( const unsigned char* const _d, const unsigned int _len, const unsigned int _w, const unsigned int _h, bool _lin = true);

        //! Find texture by name.
        TexturePtr findTexture(const std::string& _n);

        //! Find texture by OpenGL Texture ID.
        TexturePtr findTexture(const GLuint& _i);

        //! Draw text
        /*!
            This is an alias to getFont()->drawText().
			Draws the given string at the given position with the given color using the current font. This a geometry factory.
            \sa loadFont()
        */
        BatchGeometryPtr drawText( const std::string& _s, const Vector2d& _p, const Color& _c=Color(255,255,255));

        //! Delegates to BatchRenderer::getView()
        inline View& getView() { return renderer.getView(); }

        //! Delegates to BatchRenderer::setView()
        inline void setView( const View& _v ) { renderer.setView( _v ); }

        //! Delegates to GraphicsFactory2d::getDepth()
        inline float getDepth(){ return factory.getDepth(); }

        //! Delegates to GraphicsFactory2d::setDepth()
        inline void setDepth( float _d = 0.0f ) { factory.setDepth( _d ); }
            
        //! Delegates to GraphicsFactory2d::drawLine()
	    inline BatchGeometryPtr drawLine(const Vector2d& _v1 = Vector2d(0,0), const Vector2d& _v2 = Vector2d(0,0), const Color& _a = Color(255,255,255), const Color& _b = Color(255,255,255,255))
        {
            return factory.drawLine( _v1, _v2, _a, _b );
        }

	    //! Delegates to GraphicsFactory2d::drawRay()
	    inline BatchGeometryPtr drawRay(const Vector2d& _o = Vector2d(0,0), const Vector2d& _r = Vector2d(0,0), const Color& _a = Color(255,255,255), const Color& _b = Color(255,255,255,255))
        {
            return factory.drawRay( _o, _r, _a, _b );
        }

	    //! Delegates to GraphicsFactory2d::drawRectangle()
        inline BatchGeometryPtr drawRectangle( const Rectangle& _r = Rectangle(0,0,0,0), const Color& _a = Color(255,255,255), const Color& _b = Color(255,255,255), const Color& _c = Color(255,255,255), const Color& _d = Color(255,255,255) )
        {
            return factory.drawRectangle( _r, _a, _b, _c, _d );
        }

        //! Delegates to GraphicsFactory2d::drawPolygon()
        inline BatchGeometryPtr drawPolygon (const Polygon& _p, const Color& _a = Color(255,255,255))
        {
            return factory.drawPolygon( _p, _a );
        }

        //! Delegates to GraphicsFactory2d::drawTexturedPolygon()
        inline BatchGeometryPtr drawTexturedPolygon (const Polygon& _p, TexturePtr _t, const Color& _c = Color(255,255,255), bool _e = false)
        {
            return factory.drawTexturedPolygon( _p, _t, _c, _e );
        }

        //! Delegates to GraphicsFactory2d::drawTexture()
        inline BatchGeometryPtr drawTexture( TexturePtr _t, const Vector2d& _p, const RotationMatrix& _rot = RotationMatrix( 0.0f ), const Vector2d& _scale = Vector2d(1.0f,1.0f), const Color& _color = Color(255,255,255), unsigned int _flags = EGF_NONE )
        {
            return factory.drawTexture( _t, _p, _rot, _scale, _color, _flags );
        }

        //! Delegates to GraphicsFactory2d::drawTexturePart()
        inline BatchGeometryPtr drawTexturePart( TexturePtr _t, const Vector2d& _p, const Rectangle& _rect, const RotationMatrix& _rot = RotationMatrix(0.0f), const Vector2d& _scale=Vector2d(1.0f,1.0f), const Color& _color=Color(255,255,255), unsigned int _flags = EGF_NONE )
        {
            return factory.drawTexturePart( _t, _p, _rect, _rot, _scale, _color, _flags );
        }

    private:

        //! Batch Renderer
        BatchRenderer renderer;

        //! Graphics Factory.
        GraphicsFactory2d factory;

        //! Resource manager
        ResourceManager resources;

		//! Debug Console.
		boost::shared_ptr<DebugConsole> console;

        //! The system font used to draw text
        FontPtr font;

		//! Quit variable
		bool quit;

		//! Window event connection
		boost::signals2::connection event_connection;

		//! Resize switch
		bool resize;

        //! Timer for FPS.
        Timer fpstimer;

        //! Counts the number of frames per second
        double framerate;

    };

} //namespace phoenix

#endif //__PHOENIXRS__
