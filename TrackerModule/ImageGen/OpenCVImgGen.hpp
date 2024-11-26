
#include <opencv2/opencv.hpp>
#include <string.h>

namespace OpenCVImgGen {

    void saveImage (std::string imageSavePath, const cv::Mat & imageToSave) {
        if (!(cv::imwrite(imageSavePath, imageToSave))) 
            std::cerr << "Error while saving image \"" << imageSavePath << "\" : could not save" << std::endl;
    }



    void loadImage(std::string imagePath, int loadType, cv::Mat & inputImage) {
        inputImage = cv::imread(imagePath, loadType);
        if (inputImage.empty()) {
            std::cerr << "Error loading image path \"" << imagePath << "\": image not found" << std::endl;
        }
    }

    void saveImages (const std::initializer_list <cv::Mat> & images, const std::initializer_list <std::string> & imagePaths) {
        auto imagesIterator = images.begin();
        auto imagePathsIterator = imagePaths.begin();
        for (unsigned char i = 0; i < images.size(); i++) {
            saveImage(* imagePathsIterator, * imagesIterator);
            imagePathsIterator++;
            imagesIterator++;
        }
    }

    void showImages (const std::initializer_list <cv::Mat> & images, const std::initializer_list <std::string> & windowNames) {
        auto imagesIterator = images.begin();
        auto windowNamesIterator = windowNames.begin();
        for (unsigned char i = 0; i < images.size(); i++) {
            cv::imshow(* windowNamesIterator, * imagesIterator);
            windowNamesIterator++;
            imagesIterator++;
        }
        cv::waitKey(1);
    }

    void releaseImages (const std::initializer_list <cv::Mat> & images) {
        for (auto singleImage : images) singleImage.release();
    }
};