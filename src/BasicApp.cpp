#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

// nbody includes
#include "bh_tree.h"
#include "body_builder.h"
#include "nbody_cinder.h"



// extra cinder includes
//#include "cinder/ImageIo.h"
//#include "cinder/gl/Texture.h"


//#include <ctime>
//#include <iostream>

using namespace ci;
using namespace ci::app;
//
// We'll create a new Cinder Application by deriving from the App class.
class BasicApp : public App {
public:
    // Cinder will call 'mouseDrag' when the user moves the mouse while holding one of its buttons.
    // See also: mouseMove, mouseDown, mouseUp and mouseWheel.
    void mouseDrag( MouseEvent event ) override;

    // Cinder will call 'keyDown' when the user presses a key on the keyboard.
    // See also: keyUp.
    void keyDown( KeyEvent event ) override;

    // Cinder will call 'draw' each time the contents of the window need to be redrawn.
    void draw() override;

    virtual void setup() override;

    virtual void update() override;
    //gl::Texture my_image;

private:
    // This will maintain a list of points which we will draw line segments between
    std::vector<vec2> mPoints;
    std::vector<std::shared_ptr<body>> bodies;

    bool go_go_go;
    bool draw_velocity;
    bool draw_as_line;
    bool draw_bodies;

    std::vector<int> body_numbers { 10, 15, 25, 50, 100, 250, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000};
    int body_number_index = 7;

    std::vector<clock_t> times;

    // todo: add the region to the Basic APP
    region tree_region;
    region draw_region;

};

void prepareSettings( BasicApp::Settings* settings )
{
	settings->setMultiTouchEnabled( false );
}

void BasicApp::mouseDrag( MouseEvent event )
{
	// Store the current mouse position in the list.
	mPoints.push_back( event.getPos() );
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

    // my_image = gl::Texture( loadImage( loadResource(filename)));
    // gl::draw( my_image, getWindowBounds());


    gl::color( 0.0f, 0.0f, 1.0f);
    if (draw_bodies) {
        for (auto &e : bodies) {
            gl::color( 0.0f, 0.0f, 1.0f);
            if (e->get_mass() > 10000) {
                gl::color( 1.0f, 0.0f, 0.0f);
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
	gl::begin( GL_LINE_STRIP );
	for( const vec2 &point : mPoints ) {
		gl::vertex( point );
	}
	gl::end();


}


void BasicApp::setup() {
    //AppBase::setup();
    //body_test_1(bodies);

    tree_region.set(-1e4, -1e4, 1e4, 1e4);
    draw_region.set( -2.5e3, -2.5e3, 2.5e3, 2.5e3);

    //many_bodies_test(bodies);
    auto center = getWindowCenter();
    add_galaxy_to_body_list(bodies, vec2_to_point(center));

    //region galaxy_region(-1e2, -1e2, 1e2, 1e2);
    create_two_galaxies(bodies, draw_region);

    go_go_go = false;
    draw_velocity = false;
    draw_as_line = false;

    draw_bodies = true;

    // find max x and y vels
    //
    double max_x_vel = 0, max_y_vel = 0;


    // todo: remove test code below when un-needed
    for (auto &b : bodies) {
        auto velx = b->get_velocity().x();
        auto vely = b->get_velocity().y();
        max_x_vel = velx > max_x_vel ? velx : max_x_vel;
        max_y_vel = vely > max_y_vel ? vely : max_y_vel;
    }

    std::cout << "max x vel: " << max_x_vel << ", and max_y_vel: " << max_y_vel << std::endl;
    std::cout << "we are working with " << bodies.size() << " bodies" << std::endl;

}

void BasicApp::update() {
    //AppBase::update();

    if (go_go_go) {
        auto t = clock();

        region r( -1e6, -1e6, 1e6, 1e6);
        auto forces = compute_forces(bodies, r);
        update_bodies_with_forces(bodies, forces);

        //t = clock() - t;
        //times.push_back(t);
        //double sum = 0.0;
        //for (auto &e : times) {
        //    sum += t;
        //}
        //sum /= CLOCKS_PER_SEC;
        //sum /= times.size();
        //std::cout << "average time: " << sum << ", for number of bodies = " << bodies.size() << std::endl;

    }

}


// This line tells Cinder to actually create and run the application.

CINDER_APP( BasicApp, RendererGl, prepareSettings )
