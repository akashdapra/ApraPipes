#include <boost/test/unit_test.hpp>

#include "NvArgusCamera.h"
#include "NvTransform.h"
#include "FileWriterModule.h"
#include "H264EncoderV4L2.h"
#include "VirtualCameraSink.h"
#include "RTSPPusher.h"
#include "StatSink.h"
#include "PipeLine.h"
#include "DMAFDToHostCopy.h"

BOOST_AUTO_TEST_SUITE(nvarguscamera_tests)

BOOST_AUTO_TEST_CASE(basic, *boost::unit_test::disabled())
{
	NvArgusCameraProps sourceProps(1280, 720);
	sourceProps.maxConcurrentFrames = 10;
	sourceProps.fps = 120;
	auto source = boost::shared_ptr<Module>(new NvArgusCamera(sourceProps));

	StatSinkProps sinkProps;
	sinkProps.logHealth = true;
	sinkProps.logHealthFrequency = 100;
	auto sink = boost::shared_ptr<Module>(new StatSink(sinkProps));
	source->setNext(sink);

	PipeLine p("test");
	p.appendModule(source);
	BOOST_TEST(p.init());

	Logger::setLogLevel(boost::log::trivial::severity_level::info);

	p.run_all_threaded();

	boost::this_thread::sleep_for(boost::chrono::seconds(10));
	Logger::setLogLevel(boost::log::trivial::severity_level::error);

	p.stop();
	p.term();

	p.wait_for_all();
}

BOOST_AUTO_TEST_CASE(vcam_nv12, *boost::unit_test::disabled())
{
	NvArgusCameraProps sourceProps(1280, 720);
	sourceProps.maxConcurrentFrames = 10;
	sourceProps.fps = 30;
	auto source = boost::shared_ptr<Module>(new NvArgusCamera(sourceProps));

	auto copySource = boost::shared_ptr<Module>(new DMAFDToHostCopy);
	source->setNext(copySource);
	
	VirtualCameraSinkProps sinkProps("/dev/video10");
	sinkProps.logHealth = true;
	sinkProps.logHealthFrequency = 100;
	auto sink = boost::shared_ptr<Module>(new VirtualCameraSink(sinkProps));
	copySource->setNext(sink);

	// auto fileWriter = boost::shared_ptr<Module>(new FileWriterModule(FileWriterModuleProps("./data/testOutput/nvargus/nv12_????.raw")));
	// source->setNext(fileWriter);

	PipeLine p("test");
	p.appendModule(source);
	BOOST_TEST(p.init());

	Logger::setLogLevel(boost::log::trivial::severity_level::info);

	p.run_all_threaded();

	boost::this_thread::sleep_for(boost::chrono::seconds(1000));
	Logger::setLogLevel(boost::log::trivial::severity_level::error);

	p.stop();
	p.term();

	p.wait_for_all();
}

BOOST_AUTO_TEST_CASE(vcam_yuv420, *boost::unit_test::disabled())
{
	NvArgusCameraProps sourceProps(1280, 720);
	sourceProps.maxConcurrentFrames = 10;
	sourceProps.fps = 30;
	auto source = boost::shared_ptr<Module>(new NvArgusCamera(sourceProps));

	auto copySource = boost::shared_ptr<Module>(new DMAFDToHostCopy);
	source->setNext(copySource);

	auto transform = boost::shared_ptr<Module>(new NvTransform(NvTransformProps(ImageMetadata::YUV420)));
	source->setNext(transform);

	auto copy = boost::shared_ptr<Module>(new DMAFDToHostCopy);
	transform->setNext(copy);

	VirtualCameraSinkProps sinkProps("/dev/video10");
	sinkProps.logHealth = true;
	sinkProps.logHealthFrequency = 100;
	auto sink = boost::shared_ptr<Module>(new VirtualCameraSink(sinkProps));
	copy->setNext(sink);

	// auto fileWriter = boost::shared_ptr<Module>(new FileWriterModule(FileWriterModuleProps("./data/testOutput/nvargus/yuv420_????.raw")));
	// copy->setNext(fileWriter);
	// auto fileWriter2 = boost::shared_ptr<Module>(new FileWriterModule(FileWriterModuleProps("./data/testOutput/nvargus/nv12_????.raw")));
	// copySource->setNext(fileWriter2);

	PipeLine p("test");
	p.appendModule(source);
	BOOST_TEST(p.init());

	Logger::setLogLevel(boost::log::trivial::severity_level::info);

	p.run_all_threaded();

	boost::this_thread::sleep_for(boost::chrono::seconds(100));
	Logger::setLogLevel(boost::log::trivial::severity_level::error);

	p.stop();
	p.term();

	p.wait_for_all();
}

BOOST_AUTO_TEST_CASE(vcam, *boost::unit_test::disabled())
{
	NvArgusCameraProps sourceProps(1280, 720);
	sourceProps.maxConcurrentFrames = 10;
	sourceProps.fps = 30;
	auto source = boost::shared_ptr<Module>(new NvArgusCamera(sourceProps));

	auto copySource = boost::shared_ptr<Module>(new DMAFDToHostCopy);
	source->setNext(copySource);

	auto transform = boost::shared_ptr<Module>(new NvTransform(NvTransformProps(ImageMetadata::BGRA)));
	source->setNext(transform);

	auto copy = boost::shared_ptr<Module>(new DMAFDToHostCopy);
	transform->setNext(copy);

	VirtualCameraSinkProps sinkProps("/dev/video10");
	sinkProps.logHealth = true;
	sinkProps.logHealthFrequency = 100;
	auto sink = boost::shared_ptr<Module>(new VirtualCameraSink(sinkProps));
	copy->setNext(sink);

	// auto fileWriter = boost::shared_ptr<Module>(new FileWriterModule(FileWriterModuleProps("./data/testOutput/nvargus/bgra_????.raw")));
	// copy->setNext(fileWriter);
	// auto fileWriter2 = boost::shared_ptr<Module>(new FileWriterModule(FileWriterModuleProps("./data/testOutput/nvargus/nv12_????.raw")));
	// copySource->setNext(fileWriter2);

	PipeLine p("test");
	p.appendModule(source);
	BOOST_TEST(p.init());

	Logger::setLogLevel(boost::log::trivial::severity_level::info);

	p.run_all_threaded();

	boost::this_thread::sleep_for(boost::chrono::seconds(100));
	Logger::setLogLevel(boost::log::trivial::severity_level::error);

	p.stop();
	p.term();

	p.wait_for_all();
}

BOOST_AUTO_TEST_CASE(encoder, *boost::unit_test::disabled())
{
	NvArgusCameraProps sourceProps(1280, 720);
	sourceProps.maxConcurrentFrames = 10;
	sourceProps.fps = 30;
	auto source = boost::shared_ptr<Module>(new NvArgusCamera(sourceProps));
	
	H264EncoderV4L2Props encoderProps;
	encoderProps.targetKbps = 4*1024;
	auto encoder = boost::shared_ptr<Module>(new H264EncoderV4L2(encoderProps));
	source->setNext(encoder);

	auto fileWriter = boost::shared_ptr<Module>(new FileWriterModule(FileWriterModuleProps("./data/testOutput/ArgusCamera/frame.h264", true)));
	encoder->setNext(fileWriter);

	StatSinkProps sinkProps;
	sinkProps.logHealth = true;
	sinkProps.logHealthFrequency = 100;
	auto sink = boost::shared_ptr<Module>(new StatSink(sinkProps));
	encoder->setNext(sink);

	PipeLine p("test");
	p.appendModule(source);
	BOOST_TEST(p.init());

	Logger::setLogLevel(boost::log::trivial::severity_level::info);

	p.run_all_threaded();

	boost::this_thread::sleep_for(boost::chrono::seconds(100));
	Logger::setLogLevel(boost::log::trivial::severity_level::error);

	p.stop();
	p.term();

	p.wait_for_all();
}

BOOST_AUTO_TEST_CASE(encoderrtsppush, *boost::unit_test::disabled())
{
	NvArgusCameraProps sourceProps(1280, 720);
	sourceProps.maxConcurrentFrames = 10;
	sourceProps.fps = 30;
	auto source = boost::shared_ptr<Module>(new NvArgusCamera(sourceProps));
	
	H264EncoderV4L2Props encoderProps;
	encoderProps.targetKbps = 4*1024;
	auto encoder = boost::shared_ptr<Module>(new H264EncoderV4L2(encoderProps));
	source->setNext(encoder);

	// auto fileWriter = boost::shared_ptr<Module>(new FileWriterModule(FileWriterModuleProps("./data/testOutput/ArgusCamera_1280x720.h264", true)));
	// encoder->setNext(fileWriter);

	RTSPPusherProps rtspPusherProps("rtsp://10.102.10.129:5544", "aprapipes_h264");
	auto rtspPusher = boost::shared_ptr<Module>(new RTSPPusher(rtspPusherProps));
	encoder->setNext(rtspPusher);

	StatSinkProps sinkProps;
	sinkProps.logHealth = true;
	sinkProps.logHealthFrequency = 100;
	auto sink = boost::shared_ptr<Module>(new StatSink(sinkProps));
	encoder->setNext(sink);

	PipeLine p("test");
	p.appendModule(source);
	BOOST_TEST(p.init());

	Logger::setLogLevel(boost::log::trivial::severity_level::info);

	p.run_all_threaded();

	boost::this_thread::sleep_for(boost::chrono::seconds(100000));
	Logger::setLogLevel(boost::log::trivial::severity_level::error);

	p.stop();
	p.term();

	p.wait_for_all();
}

BOOST_AUTO_TEST_SUITE_END()
