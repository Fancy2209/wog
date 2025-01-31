#include "GooBall.h"
#include "Boy/Environment.h"

Wog *GooBall::spWogInstance = NULL;

GooBall::GooBall() {};

GooBall::~GooBall() {};

void GooBall::init() { 
	spWogInstance = Wog::instance();
};