import cv2
import skimage
import os

# Resolution of the display
resolution = (32, 32)

# Resolution of the playback preview
playback_resolution = (720, 720)

# FPS of the final video, this can be changed to speed up or slow down the video
fps = 60

# The videos directory should contain all the videos to be processed
input_folder = "videos"
output_folder = "output"

# The file name of the processed videos looks like:
# [video_name]_processed_[resolution]_[fps].wdis
# Example: video_processed_32x32_60.wdis

# Inside a processed video file, each line is a frame in the video with each pixel being a 3x1 vector
# Ex: [r, g, b] [r, g, b] [r, g, b] ... [r, g, b]


def process_video(input, output_folder, preview=False):
    capture = cv2.VideoCapture(input)

    # Loop through the video frames
    output_rows = []
    while True:
        ret, frame = capture.read()
        if ret == True:
            resized_frame = cv2.resize(frame, resolution, fx=0, fy=0, interpolation = cv2.INTER_AREA)
            processed_frame = skimage.util.img_as_ubyte(resized_frame)

            # Iterate over all the pixels in the processed frame and generate the rows of the output file
            output_row = ""
            for i, row in enumerate(processed_frame):
                for j, pixel in enumerate(row):
                    output_row += "[" + str(pixel[0]) + "," + str(pixel[1]) + "," + str(pixel[2]) + "]" + (" " if (j < len(row) - 1) or (i < len(processed_frame) - 1) else "")
            
            output_rows.append(output_row)


            if preview:
                upscaled_processed_frame = cv2.resize(processed_frame, playback_resolution, fx=0, fy=0, interpolation = cv2.INTER_NEAREST)

                cv2.imshow("frame", upscaled_processed_frame)
                key = cv2.waitKey(int((1 / fps) * 1000)) 
                if key == ord('q'):
                    break
        else:
            break

    # Write the output file
    output_file_name = input.split("/")[-1].split(".")[0] + "_processed_" + str(resolution[0]) + "x" + str(resolution[1]) + "_" + str(fps) + ".wdis"
    output_file = open(output_folder + "/" + output_file_name, "w")
    for row in output_rows:
        output_file.write(row + "\n")
    output_file.close()

    capture.release()
    cv2.destroyAllWindows()

# Play all the files in the videos directory
for file in os.listdir(input_folder):
    process_video("videos/" + file, output_folder, preview=True)
