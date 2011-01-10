#include "UserDetector.h"
#include "util.h"

static UserDetector* this_cast(void* that) {
	return static_cast<UserDetector*>(that);
}

static void XN_CALLBACK_TYPE s_onNewUser(UserGenerator& userGen, XnUserID userID, void* that)
{
	this_cast(that)->onNewUser(userID);
}

static void XN_CALLBACK_TYPE s_onLostUser(UserGenerator& userGen, XnUserID userID, void* that)
{
	this_cast(that)->onLostUser(userID);
}

static void XN_CALLBACK_TYPE s_onCalibrationStart(SkeletonCapability& capability, XnUserID userID, void* that)
{
	this_cast(that)->onCalibrationStart(userID);
}

static void XN_CALLBACK_TYPE s_onCalibrationEnd(SkeletonCapability& capability, XnUserID userID, XnBool isSuccess, void* that)
{
	this_cast(that)->onCalibrationEnd(userID, i2b(isSuccess));
}

static void XN_CALLBACK_TYPE s_onPoseStart(PoseDetectionCapability& capability, const XnChar* pose, XnUserID userID, void* that)
{
	this_cast(that)->onPoseStart(userID, pose);
}

static void XN_CALLBACK_TYPE s_onPoseEnd(PoseDetectionCapability& capability, const XnChar* pose, XnUserID userID, void* that)
{
	this_cast(that)->onPoseEnd(userID, pose);
}

UserDetector::UserDetector(UserGenerator* userGen)
{
	m_listener = NULL;
	m_userGen = userGen;
	CHECK_ERROR(userGen->IsCapabilitySupported(XN_CAPABILITY_SKELETON), "This configuration is not supported.");

	m_userGen->RegisterUserCallbacks(s_onNewUser, s_onLostUser, this, m_hUserCallbacks);
	m_userGen->GetSkeletonCap().RegisterCalibrationCallbacks(s_onCalibrationStart, s_onCalibrationEnd, this, m_hCalibrationCallbacks);

	if (userGen->GetSkeletonCap().NeedPoseForCalibration()) {
		CHECK_ERROR(userGen->IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION), "This configuration is not supported.");

		m_pose = new XnChar[20];
		userGen->GetSkeletonCap().GetCalibrationPose(m_pose);
		// printf("Calibration Pose = %s\n", m_pose);

		userGen->GetPoseDetectionCap().RegisterToPoseCallbacks(s_onPoseStart, s_onPoseEnd, this, m_hPoseCallbacks);
	} else {
		m_pose = NULL;
		m_hPoseCallbacks = NULL;
	}
}

UserDetector::~UserDetector()
{
	delete[] m_pose;
}

void UserDetector::addListener(UserListener* listener)
{
	assert(m_listener == NULL); // TODO only one listener is supported for now
	m_listener = listener;
}

XV3 UserDetector::getSkeletonJointPosition(XnSkeletonJoint eJoint)
{
	XnSkeletonJointPosition j;
	getSkeletonJointPosition(eJoint, &j);

	if (isConfident(j)) {
		m_skeletonPositions[eJoint-1] = j;
	}
	return m_skeletonPositions[eJoint-1].position;
}

void UserDetector::getSkeletonJointPosition(XnSkeletonJoint eJoint, XnSkeletonJointPosition* pJointPosition)
{
	XnUserID userID = getTrackedUserID();
	if (userID) {
		m_userGen->GetSkeletonCap().GetSkeletonJointPosition(userID, eJoint, *pJointPosition);
	} else {
		pJointPosition->fConfidence = 0;
	}
}

void UserDetector::startCalibration(XnUserID userID)
{
	if (m_pose) {
		m_userGen->GetPoseDetectionCap().StartPoseDetection(m_pose, userID);
	} else {
		m_userGen->GetSkeletonCap().RequestCalibration(userID, TRUE);
	}
}

void UserDetector::onNewUser(XnUserID userID)
{
	printf("User %d : New\n", userID);

	if (m_listener) {
		m_listener->onNewUser(userID);
	}

	startCalibration(userID);
}

void UserDetector::onLostUser(XnUserID userID)
{
	printf("User %d : Lost\n", userID);

	if (m_listener) {
		m_listener->onLostUser(userID);
	}
}

void UserDetector::onCalibrationStart(XnUserID userID)
{
	printf("User %d : Calibration Start\n", userID);

	if (m_listener) {
		m_listener->onCalibrationStart(userID);
	}
}

void UserDetector::onCalibrationEnd(XnUserID userID, bool isSuccess)
{
	printf("User %d : Calibration %s\n", userID, isSuccess ? "Succseeded" : "Failed");

	if (m_listener) {
		m_listener->onCalibrationEnd(userID, isSuccess);
	}

	if (isSuccess) {
		m_userGen->GetSkeletonCap().StartTracking(userID);
	} else {
		startCalibration(userID);
	}
}

void UserDetector::onPoseStart(XnUserID userID, const XnChar* pose)
{
	printf("User %d : Pose %s Detected\n", userID, pose);

	if (m_listener) {
		m_listener->onPoseStart(userID, pose);
	}

	m_userGen->GetPoseDetectionCap().StopPoseDetection(userID);
	m_userGen->GetSkeletonCap().RequestCalibration(userID, TRUE);
}

void UserDetector::onPoseEnd(XnUserID userID, const XnChar* pose)
{
	printf("User %d : Pose %s End\n", userID, pose);

	if (m_listener) {
		m_listener->onPoseEnd(userID, pose);
	}
}

XnUserID UserDetector::getTrackedUserID()
{
	XnUserID userIDs[15];
	XnUInt16 numUsers = 15;
	m_userGen->GetUsers(userIDs, numUsers);
	if (numUsers == 0) {
		return false;
	}

	for (XnUInt16 i = 0; i < numUsers; i++) {
		XnUserID userID = userIDs[i];
		if (m_userGen->GetSkeletonCap().IsTracking(userID)) {
			return userID;
		}
	}
	return 0;
}

void UserDetector::stopTracking()
{
	XnUserID userID = getTrackedUserID();
	if (userID) {
		m_userGen->GetSkeletonCap().StopTracking(userID);
		startCalibration(userID);
	}
}

XV3 UserDetector::getForwardVector()
{
	XnUserID userID = getTrackedUserID();
	if (userID) {
		// OpenNI's orientation does not work very well.
		/*
		XnSkeletonJointTransformation t;
		m_userGen->GetSkeletonCap().GetSkeletonJoint(userID, XN_SKEL_TORSO, t);
		float* e = t.orientation.orientation.elements;
		return XV3(e[6], e[7], -e[8]);
		*/

		XnSkeletonJointPosition p0, p1, p2;
		m_userGen->GetSkeletonCap().GetSkeletonJointPosition(userID, XN_SKEL_RIGHT_SHOULDER, p0);
		m_userGen->GetSkeletonCap().GetSkeletonJointPosition(userID, XN_SKEL_TORSO, p1);
		m_userGen->GetSkeletonCap().GetSkeletonJointPosition(userID, XN_SKEL_LEFT_SHOULDER, p2);
		XV3 v0(p0.position), v1(p1.position), v2(p2.position);
		return ((v1 - v0).cross(v2 - v1)).normalize();
	} else {
		return XV3();
	}
}

XV3 UserDetector::getUpVector()
{
	XnUserID userID = getTrackedUserID();
	if (userID) {
		XnSkeletonJointPosition p0, p1;
		m_userGen->GetSkeletonCap().GetSkeletonJointPosition(userID, XN_SKEL_TORSO, p0);
		m_userGen->GetSkeletonCap().GetSkeletonJointPosition(userID, XN_SKEL_NECK, p1);
		XV3 v0(p0.position), v1(p1.position);
		return (v1 - v0).normalize();
	} else {
		return XV3();
	}
}
