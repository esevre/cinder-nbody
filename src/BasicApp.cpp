//
// Created by Erik Sevre on 10/25/16.
//
// todo: write summary of app
// todo: comment the program to make everything clear
//

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Text.h"
#include "cinder/gl/Texture.h"
#include "cinder/Utilities.h"
#include "cinder/ImageIo.h"


// nbody includes
#include "bh_tree.h"
#include "body_builder.h"
#include "nbody_cinder.h"

// used for writing number of bodies to screen
#include <sstream>


//#include <ctime>
//#include <iostream>

using namespace ci;
using namespace ci::app;

//
// Create a "Basic App" from the default "App" class in cinder
//
class BasicApp : public App {
public:
    //  ** mouseDrag is Left over from default code I started with
    //  ** I want to use this to add more bodies in the future, for a program I want to play with
    //  **
    // Cinder will call 'mouseDrag' when the user moves the mouse while holding one of its buttons.
    // See also: mouseMove, mouseDown, mouseUp and mouseWheel.
    //
    void mouseDrag( MouseEvent event ) override;

    //  ** keyDown() was in the default code, but I adapted it to control what is being visualized
    //
    // Cinder will call 'keyDown' when the user presses a key on the keyboard.
    // See also: keyUp.
    void keyDown( KeyEvent event ) override;


    //
    //  Core of an app: setup - update - draw
    //
    // setup()  : is called once when the app starts
    // update() : called before each frame is drawn
    // draw()   : called to draw each frame repeatedly
    //
    virtual void setup() override;
    virtual void update() override;
    void draw() override;

    //
    //  Variables for on screen text
    //
    gl::Texture2dRef mTexture; // Texture reference (for the text)


private:
    // This will maintain a list of points which we will draw line segments between
    std::vector<vec2> mPoints;
    std::vector<std::shared_ptr<body>> bodies;

    bool go_go_go;
    bool draw_velocity;
    bool draw_as_line;
    bool draw_bodies;

    std::vector<int> body_numbers { 10, 15, 25, 50, 100, 250, 500, 1000, 2000, 3000, 4000, 5000, 10000, 25000, 50000, 100000, 250000};
    int body_number_index = 6;

    // timing variables
    std::vector<clock_t> times;
    double fps;
    double frame_draw_time;
    // double avg_draw_time;

    // todo: add the region to the Basic APP
    region tree_region;
    region draw_region;

    void run_multigalaxy();
    void run_single_galaxy();
};

void prepareSettings( BasicApp::Settings* settings )
{
	settings->setMultiTouchEnabled( false );
}

void BasicApp::mouseDrag( MouseEvent event )
{
	// Store the current mouse position in the list.
	// mPoints.push_back( event.getPos() );
    add_body_to_bodies(bodies, getWindowSize(), event.getPos(), draw_region);
}

void BasicApp::keyDown( KeyEvent event )
{
	std::cout << event.getCode() << " : " << event.getChar() << std::endl;

    if( event.getChar() == 'f' ) {
        // Toggle full screen when the user presses the 'f' key.
        setFullScreen( ! isFullScreen() );
    } else if( event.getCode() == KeyEvent::KEY_SPACE ) {
		// Clear the list of points when the user presses the space bar.
		mPoints.clear();
	} else if( event.getCode() == KeyEvent::KEY_ESCAPE ) {
		// Exit full screen, or quit the application, when the user presses the ESC key.
		if( isFullScreen() )
			setFullScreen( false );
		else
			quit();
	} else if (event.getCode() == 'n') {
        region r( -1e6, -1e6, 1e6, 1e6);
        std::cout << "bodies before: " << bodies.size() << ", bodies after: ";
        auto forces = compute_forces(bodies, r);
        update_bodies_with_forces(bodies, forces);
        std::cout << bodies.size() << std::endl;

    } else if (event.getCode() == 'g') {
        go_go_go = !go_go_go;
    } else if (event.getCode() == 'l') {
        draw_as_line = !draw_as_line;
    } else if (event.getCode() == 'v') {
        draw_velocity = !draw_velocity;
    } else if (event.getCode() == 'b') {
        draw_bodies = !draw_bodies;
    } else if (event.getCode() == KeyEvent::KEY_UP ) {
        bodies.clear();
        if (body_number_index < body_numbers.size()-1) {
            ++body_number_index;
        }
        many_bodies_test(bodies, body_numbers[body_number_index]);

    } else if (event.getCode() == KeyEvent::KEY_DOWN ) {
        bodies.clear();
        if (body_number_index > 0) {
            --body_number_index;
        }
        many_bodies_test(bodies, body_numbers[body_number_index]);

    } else if (event.getCode() == 'm') {
        run_multigalaxy();
    }

}

//
//
//  todo:  update the code in draw, so that the image is not distorted with the screen
//
void BasicApp::draw()
{
	// Clear the contents of the window. This call will clear
	// both the color and depth buffers.
	//gl::clear( Color::gray( 0.1f ) );
    float gray = sinf( getElapsedSeconds() ) * 0.5f + 0.5f;
	//gl::clear( Color(gray, gray, gray), true);
    gl::clear();


    gl::color( 0.0f, 0.0f, 1.0f);
    if (draw_bodies) {
        for (auto &e : bodies) {
            gl::color( 0.0f, 0.2f, 1.0f);
            if (e->get_mass() > 10000) {
                gl::color( 1.0f, 0.1f, 0.1f);
            }
            auto pt = scale_point_to_screen(e->get_position(), draw_region, getWindowSize());
            auto mass = std::log(std::sqrt(e->get_mass()))/std::log(10);
            gl::drawSolidCircle(pt, mass);
        }
    }

    gl::color( 0.0f, 1.0f, 0.5f);
    if (draw_as_line) {
        for (auto &e : bodies) {
            gl::color( 0.0f, 1.0f, 0.5f);
            if (e->get_mass() > 10000) {
                gl::color( 0.0f, 1.0f, 1.0f);
            }
            auto pt_last = scale_point_to_screen(e->get_last_position(), draw_region, getWindowSize());
            auto pt_curr = scale_point_to_screen(e->get_position(), draw_region, getWindowSize());
            gl::begin(GL_LINE_STRIP);
            gl::vertex(pt_last);
            gl::vertex(pt_curr);
            gl::end();
        }
    }

    // draw velocity vectors
    if (draw_velocity) {
        for (auto &e : bodies) {
            gl::color( 0.0f, 1.0f, 0.5f);
            if (e->get_mass() > 10000) {
                gl::color( 0.0f, 1.0f, 1.0f);
            }
            auto pt = e->get_position();
            double scale = 40000.0;
            auto pt_pos = scale_point_to_screen(pt, draw_region, getWindowSize());
            auto pt_vel = scale_point_to_screen(pt + e->get_velocity()*scale, draw_region, getWindowSize());
            gl::begin(GL_LINE_STRIP);
            gl::vertex(pt_pos);
            gl::vertex(pt_vel);
            gl::end();

        }
    }


    // Set the current draw color to orange by setting values for
	// red, green and blue directly. Values range from 0 to 1.
	// See also: gl::ScopedColor
	gl::color( 1.0f, 0.5f, 0.25f );

	// We're going to draw a line through all the points in the list
	// using a few convenience functions: 'begin' will tell OpenGL to
	// start constructing a line strip, 'vertex' will add a point to the
	// line strip and 'end' will execute the draw calls on the GPU.

//    gl::begin( GL_LINE_STRIP );
//	for( const vec2 &point : mPoints ) {
//		gl::vertex( point );
//	}
//	gl::end();



    //
    //  Display number of bodies on screen
    //
    std::stringstream display_text;
    display_text << "Number of Bodies: " << bodies.size();

    //std::stringstream timing_display_01;
    std::stringstream timing_display_02;
    std::stringstream timing_display_03;
    //timing_display_01 << "avg draw time: " << avg_draw_time;
    timing_display_02 << "last draw time: " << frame_draw_time;
    timing_display_03 << "fps: " << fps;


    TextLayout layout;                               // controls the layout

    layout.clear(ColorA(0.1f, 0.1f, 0.1f, 0.7f));
    layout.setColor(Color(0.1f, 0.9f, 0.9f));
    layout.setFont( Font("Arial Black", 16));
    layout.addCenteredLine(display_text.str());
    //layout.addCenteredLine(timing_display_01.str());
    layout.addCenteredLine(timing_display_02.str());
    layout.addCenteredLine(timing_display_03.str());


    Surface8u rendered = layout.render( true, true);
    mTexture = gl::Texture2d::create( rendered );
    gl::draw(mTexture, vec2(10,10));


}

//
//  Set up the variables before the update-draw cycle begins
//
//
//
void BasicApp::setup() {
    tree_region.set(-1e4, -1e4, 1e4, 1e4);
    draw_region.set( -2.5e3, -2.5e3, 2.5e3, 2.5e3);

    create_two_galaxies(bodies, draw_region);

    go_go_go = false;
    draw_velocity = false;
    draw_as_line = false;

    draw_bodies = true;

    fps = 0;
    frame_draw_time = 0;
}

void BasicApp::update() {
    //AppBase::update();

    if (go_go_go) {
        auto t = clock();

        region r( -1e6, -1e6, 1e6, 1e6);
        auto forces = compute_forces(bodies, r);
        update_bodies_with_forces(bodies, forces);

        t = clock() - t;
        /*
        times.push_back(t);
        double sum = 0.0;
        for (auto &e : times) {
            sum += t;
        }
        sum /= CLOCKS_PER_SEC;
        sum /= times.size();
        */
        // update time vars for on-screen display
        //avg_draw_time = sum;
        frame_draw_time = (double) t / (double) CLOCKS_PER_SEC;
        fps = 1 / frame_draw_time;

        // std::cout << "average time: " << sum << ", for number of bodies = " << bodies.size() << std::endl;

    }

}


void BasicApp::run_multigalaxy() {
    tree_region.set(-1e4, -1e4, 1e4, 1e4);
    draw_region.set( -2.5e3, -2.5e3, 2.5e3, 2.5e3);

    auto center = getWindowCenter();
    bodies.clear();
    create_two_galaxies(bodies, draw_region);

    create_two_galaxies(bodies, draw_region);
}

// This line tells Cinder to actually create and run the application.

CINDER_APP( BasicApp, RendererGl, prepareSettings )
