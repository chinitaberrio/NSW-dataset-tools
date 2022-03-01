## Synopsis

Extraction images and save new rosbag with image in it.

#### Functions included:
- Extract images from mp4
- Save a new rosbag with image in it
- rectify videos

## Code Example
	
To extract images from mp4:
```python main_mp4.py -b=ROSBAG.bag -v=VIDEO.mp4 -s=SAVE_IMG_DIR```
with flags ```-r=True``` is to rectify image ```-d=30``` will save image every 30 frames

To save a new rosbag with image i```

To rectify images from mp4 and save it as video 
```python video_rectify.py -v=VIDEO_FILE -o=OUTPUT_VIDEO```

Or, with any script, use ```-h``` for help instructions

