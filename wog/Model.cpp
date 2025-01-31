#include "Controller.h"
#include "Boy/Environment.h"

Controller::Controller() 
{
    // TODO
    BoyLib::Messenger::instance()->addListener(this);
};

Controller::~Controller() {};

bool tick() 
{
    // TODO
};

void cancel()
{
    // TODO
};

void handleMessage(const std::string &messageId, BoyLib::MessageSource *source, std::map<std::string,std::string> *params)
{
    // TODO
};