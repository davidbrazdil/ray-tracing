/*
 * Slave.cpp
 *
 *  Created on: Mar 18, 2012
 *      Author: db538
 */

#include "Slave.h"

#include <boost/asio.hpp>
using boost::asio::ip::tcp;

const char Slave::mVersion = 1;

Slave::Slave(Logger& logger, string host, unsigned short port)
	: mLogger(logger), mMaster_Name(host), mMaster_Port(port) {

	mLogger.println("SLAVE", Logger::DETAILED);
	mLogger.println(boost::format("\tHostname:       %s") % mMaster_Name, Logger::DETAILED);
	mLogger.println(boost::format("\tPort:           %d") % mMaster_Port, Logger::DETAILED);
}

Slave::~Slave() {
}

void Slave::run() {
	try {
		boost::asio::io_service io_service;
		boost::system::error_code error;

		string port = boost::str(boost::format("%d") % mMaster_Port);
		tcp::resolver resolver(io_service);
		tcp::resolver::query query(tcp::v4(), mMaster_Name, port);
		tcp::resolver::iterator iterator = resolver.resolve(query);

		mLogger.println(boost::format("Connecting to %s") % mMaster_Name, Logger::INFORMATIVE);
		tcp::socket sock(io_service);
		boost::asio::connect(sock, iterator);

		mLogger.println(boost::format("Sending handshake"), Logger::DETAILED);
		boost::array<char, sizeof(CommProtocol::HANDSHAKE)> buffer_handshake;
		memcpy(buffer_handshake.c_array(), CommProtocol::HANDSHAKE, sizeof(CommProtocol::HANDSHAKE));
		buffer_handshake[sizeof(CommProtocol::HANDSHAKE) - 1] = (signed char) Slave::mVersion;
		boost::asio::write(sock, boost::asio::buffer(buffer_handshake, sizeof(CommProtocol::HANDSHAKE)));

		mLogger.println(boost::format("Waiting for initialization"), Logger::DETAILED);
		boost::array<char, 3 * sizeof(uint32_t)> buffer_init;
		size_t length = sock.read_some(boost::asio::buffer(buffer_init), error);
		if (error == boost::asio::error::eof || length != 3 * sizeof(uint32_t))
			throw std::runtime_error(boost::str(boost::format("Unexpected EOF or wrong length")));
		else if (error)
			throw boost::system::system_error(error);
		uint32_t scene_desc_length;
		uint32_t image_width;
		uint32_t image_height;
		memcpy(&scene_desc_length, buffer_init.c_array(), sizeof(uint32_t));
		memcpy(&image_width, buffer_init.c_array() + sizeof(uint32_t), sizeof(uint32_t));
		memcpy(&image_height, buffer_init.c_array() + 2 * sizeof(uint32_t), sizeof(uint32_t));
		scene_desc_length = ntohl(scene_desc_length);
		image_width = ntohl(image_width);
		image_height = ntohl(image_height);
		mLogger.println(boost::format("Scene description length: %d") % scene_desc_length, Logger::DETAILED);
		mLogger.println(boost::format("Image width: %d") % image_width, Logger::DETAILED);
		mLogger.println(boost::format("Image height: %d") % image_height, Logger::DETAILED);

		mLogger.println(boost::format("Sending acknowledgment"), Logger::DETAILED);
		boost::array<char, 1> buffer_message;
		buffer_message[0] = CommProtocol::MSG_ACK;
		boost::asio::write(sock, boost::asio::buffer(buffer_message, sizeof(buffer_message)));

		mLogger.println(boost::format("Entering task loop"), Logger::DETAILED);
		for (;;) {
			mLogger.println(boost::format("Waiting for message"), Logger::DETAILED);
			length = sock.read_some(boost::asio::buffer(buffer_message), error);
			if (error == boost::asio::error::eof || length != sizeof(char))
				throw std::runtime_error(boost::str(boost::format("Unexpected EOF or wrong length")));
			else if (error)
				throw boost::system::system_error(error);

			if (buffer_message[0] == CommProtocol::MSG_FINISHED) {
				mLogger.println(boost::format("Task finished"), Logger::INFORMATIVE);
				break;
			} else if (buffer_message[0] == CommProtocol::MSG_WAIT) {
				mLogger.println(boost::format("Waiting..."), Logger::DETAILED);
			} else if (buffer_message[0] == CommProtocol::MSG_TASK) {
				mLogger.println(boost::format("Awaiting task"), Logger::DETAILED);
				boost::array<char, 2 * sizeof(uint32_t)> buffer_task;
				length = sock.read_some(boost::asio::buffer(buffer_task), error);
				if (error == boost::asio::error::eof || length != sizeof(buffer_task))
					throw std::runtime_error(boost::str(boost::format("Unexpected EOF or wrong length")));
				else if (error)
					throw boost::system::system_error(error);
				uint32_t col_from;
				uint32_t col_to;
				memcpy(&col_from, buffer_task.c_array(), sizeof(uint32_t));
				memcpy(&col_to, buffer_task.c_array() + sizeof(uint32_t), sizeof(uint32_t));
				col_from = ntohl(col_from);
				col_to = ntohl(col_to);
				mLogger.println(boost::format("Assigned task: %d-%d") % col_from % col_to, Logger::INFORMATIVE);

				for (unsigned int col = col_from; col < col_to; ++col)
					for (unsigned int row = 0; row < image_height; ++row) {
						mLogger.println(boost::format("Computing %dx%d") % col % row, Logger::DETAILED);
						boost::this_thread::sleep(boost::posix_time::seconds(1));
						boost::array<char, 4> buffer_result;
						buffer_result[0] = CommProtocol::MSG_OK;
						buffer_result[1] = 000;
						buffer_result[2] = 111;
						buffer_result[3] = 222;
						boost::asio::write(sock, boost::asio::buffer(buffer_result, sizeof(buffer_result)));
					}
			} else
				throw std::runtime_error(boost::str(boost::format("Unknown message from master")));
		}
	} catch (std::exception& e) {
		mLogger.println(e.what(), Logger::ERROR);
	}
}
