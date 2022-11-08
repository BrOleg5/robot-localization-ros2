#ifndef ARUCOLOCALIZATION_LIB
#define ARUCOLOCALIZATION_LIB

#ifndef PI
#	define PI 3.14159265358979323846f
#endif

#include <iostream>
#include <algorithm>
#include <cmath>

#include "opencv2/aruco.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

namespace td {

	/**
	 * @class TransferData
	 * @brief Store robot's global coordinates and local coordinate change.
	 */
	class TransferData {
		public:

		/**
		 * Current robot's global coordinate.
		 */
		cv::Point2f currGlobalCartesian;

		/**
		 * Previous robot's global coordinate.
		 */
		cv::Point2f prevGlobalCartesian;

		/**
		 * Current robot's rotation angle.
		 */
		float currAngle;

		/**
		 * Previous robot's rotation angle.
		 */
		float prevAngle;

		/**
		 * Robot's local coordinate change.
		 */
		cv::Point2f deltaEigenCartesian;

		/**
		 * Robot's angle change.
		 */
		float deltaAngle;

		cv::Point2f pixelResolution;


		TransferData() : currGlobalCartesian(0.0f, 0.0f), prevGlobalCartesian(0.0f, 0.0f), currAngle(0.0f),
						 prevAngle(0.0f), deltaAngle(0.0f), deltaEigenCartesian(0.0f, 0.0f), pixelResolution({0.f, 0.f}) {}
		/**
		 * Initializtion each coordinate as zero.
		 */
		TransferData(const cv::Point2f& pixel_resolution) : currGlobalCartesian(0.0f, 0.0f), prevGlobalCartesian(0.0f, 0.0f), currAngle(0.0f),
						 prevAngle(0.0f), deltaAngle(0.0f), deltaEigenCartesian(0.0f, 0.0f), pixelResolution(pixel_resolution) {}
		
		~TransferData() {}

		/**
		 * Obtain robot's rotation angle from aruco corners.
		 * 
		 * @param arucoCorner array of aruco corners.
		 */
		void Angle(cv::Point2f* arucoCorner);

		/**
		 * Obtain robot's angle change.
		 */
		void DeltaAngle();

		/**
		 * Obtain robot's local coordinate change.
		 */
		void DeltaEigen();
	};

	/**
	 * Calculate angle with parallel side
	*/
	float calcAngleParallelSide(cv::Point2f& p1, cv::Point2f& p2);

	/**
	 * Calculate angle with perpendicular side
	*/
	float calcAnglePerpendicularSide(cv::Point2f& p1, cv::Point2f& p2);

	/**
	 * Normalize an angle to range [0; 2pi) or [0; 360)
	*/
	float normAngle2PI(float angle, bool isRadian = true);

	/**
	 * Normalize an angle to range [-pi; pi) or [-180; 180)
	*/
	float normAnglePI(float angle, bool isRadian = true);

	float deg2rad(float deg);
	float rad2deg(float rad);
}

/**
 * @class ArucoLocalization
 * @brief Implementation robot localization by aruco marker.
 */
class ArucoLocalization {
	/**
	 * Corners of aruco marker cartesian clockwise.
	 */
	cv::Point2f arucoCorner[4];

	/**
	 * Frame size
	*/
	int frame_height;
	int frame_width;

	/**
	 * Vector for aruco markers' indexes.
	 */
	std::vector<int> markerIds;

	/**
	 * Matrix for aruco markers' actual and rejected corners.
	 */
	std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;

	/**
	 * Creating the parameters for the aruco detection.
	 */
	cv::Ptr<cv::aruco::DetectorParameters> detectorParameters;
	
	/**
	 * Selecting the dictionary to use.
	 */
	cv::Ptr<cv::aruco::Dictionary> dictionary;

	public:

	enum Status {
		NOT_MARKER_INDEX = -1,
		OK,
		END_OF_VIDEO_FILE,
		MARKER_NOT_DETECTED
	};


	ArucoLocalization();
	/**
	 * Constructor.
	 * 
	 * @param dict_name name of aruco marker dictonary.
	 */
	ArucoLocalization(cv::aruco::PREDEFINED_DICTIONARY_NAME dict_name);
	
	~ArucoLocalization() {}

	/**
	 * Localize marker.
	 */
	int detectMarkers(const cv::Mat& frame);

	int filterMarkers(int markerID);

	/**
	 * Calculate marker positions on the plane.
	 * 
	 * @param data storage marker's global coordinates and local coordinate change.
	 * @param markerID ID of marker that position calculating
	 */
	bool estimatePosition(td::TransferData* data, int markerID = -1);

	cv::Mat draw_marker(const cv::Mat& frame, int markerID = -1);

	void getMarkersCorners(std::vector<std::vector<cv::Point2f>>& marker_corners);
	void getMarkersIndexes(std::vector<int>& marker_ids);

	void setMarkerDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME dict_name);
	void setFrameSize(int frame_width, int frame_height);
};

#endif