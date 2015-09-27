#include <iostream>
#include <string>
#include <stdexcept>
#include <fstream>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <GL/freeglut.h>

#include <tbb/task_scheduler_init.h>

#include <openanim/loaders/asf.h>

#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>

#include "common/viewport.h"

/*#include "mesh.h"
#include "grid.h"
#include "factory.tpp"
#include "array_maths.h"
#include "sampler.h"

#include "../common/config.h" */
#include "common/bind.h"

namespace po = boost::program_options;

using std::cout;
using std::endl;
using std::flush;

int main(int argc, char* argv[]) {
	// Declare the supported options.
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("input", po::value<std::string>(), "input file (bvh)")
	;

	// process the options
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if(vm.count("help") || (!vm.count("input"))) {
		cout << desc << "\n";
		return 1;
	}

	cout << "Loading file " << vm["input"].as<std::string>() << " ... " << flush;
	
	// load the skeleton
	openanim::Skeleton skel = openanim::loadASF(vm["input"].as<std::string>());
	
	// convert the skeleton to world space
	for(auto& j : skel)
		if(j.hasParent())
			j.tr() = j.parent().tr() * j.tr();

	cout << "done." << endl;

	///////////////////////////////

	glutInit(&argc, argv);

	QApplication app(argc, argv);

	// make a window only with a viewport
	QMainWindow win;

	win.showMaximized();

	openanim::viewport* viewport = new openanim::viewport(&win);
	win.setCentralWidget(viewport);

	openanim::qt_bind(viewport, SIGNAL(render(float)), [&]() {
		glColor3f(0, 0.2, 0);

		glBegin(GL_LINES);

		for(int a=-10;a<=10;++a) {
			glVertex3f(a, 0, -10);
			glVertex3f(a, 0, 10);

			glVertex3f(-10, 0, a);
			glVertex3f(10, 0, a);
		}
		glEnd();

		// draw the skeleton
		glColor3f(1,1,1);
		glBegin(GL_LINES);
		for(auto& j : skel)
			if(j.hasParent()) {
				glVertex3fv(j.tr().translation.getValue());
				glVertex3fv(j.parent().tr().translation.getValue());
			}
		glEnd();

	});

	app.exec();

	return 0;
}
