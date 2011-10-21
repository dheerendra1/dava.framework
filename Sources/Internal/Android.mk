#-----------------------------
# Framework lib 

# set local path for lib
LOCAL_PATH := $(call my-dir)

# clear all variables
include $(CLEAR_VARS)

# set module name
LOCAL_MODULE := libInternal

# set path for includes
LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../Libs/include

# set exported includes
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)

# set source files 
LOCAL_SRC_FILES :=  \
                    Animation/AnimatedObject.cpp \
                    Animation/Animation.cpp \
                    Animation/AnimationManager.cpp \
                    Animation/BezierSplineAnimation.cpp \
                    Animation/Interpolation.cpp \
                    Animation/KeyframeAnimation.cpp \
                    Animation/LinearAnimation.cpp \
                    \
                    Base/BaseMath.cpp \
                    Base/BaseObjectChecker.cpp \
                    Base/DynamicObjectCache.cpp \
                    Base/EventDispatcher.cpp \
                    Base/Message.cpp \
                    Base/ObjectFactory.cpp \
                    \
                    Collision/CollisionObject2.cpp \
                    Collision/CollisionPolygons.cpp \
                    Collision/Collisions.cpp \
                    \
                    Core/ApplicationCore.cpp \
                    Core/Core.cpp \
                    \
                    Debug/MemoryManager.cpp \
                    Debug/Replay.cpp \
                    \
                    FileSystem/Bitstream.cpp \
                    FileSystem/DynamicMemoryFile.cpp \
                    FileSystem/File.cpp \
                    FileSystem/FileAndroid.cpp \
                    FileSystem/FileList.cpp \
                    FileSystem/FileSystem.cpp \
                    FileSystem/KeyedArchive.cpp \
                    FileSystem/KeyedArchiver.cpp \
                    FileSystem/KeyedUnarchiver.cpp \
                    FileSystem/LocalizationSystem.cpp \
                    FileSystem/Logger.cpp \
                    FileSystem/LoggerAndroid.cpp \
                    FileSystem/ResourceArchive.cpp \
                    FileSystem/VariantType.cpp \
                    FileSystem/YamlArchive.cpp \
                    FileSystem/YamlParser.cpp \
                    FileSystem/XMLParser.cpp \
                    \
                    Input/Accelerometer.cpp \
                    Input/AccelerometerAndroid.cpp \
                    \
                    Math/Neon/NeonMath.cpp \
                    \
                    Math/AABBox2.cpp \
                    Math/AABBox3.cpp \
                    Math/BezierSpline.cpp \
                    Math/Math2D.cpp \
                    Math/Matrix4.cpp \
                    Math/Polygon2.cpp \
                    Math/Polygon3.cpp \
                    Math/Spline.cpp \
                    \
                    Network/NetworkConnection.cpp \
                    Network/NetworkDelegate.cpp \
                    Network/NetworkPacket.cpp \
                    \
                    Particles/Particle.cpp \
                    Particles/ParticleEmitter.cpp \
                    Particles/ParticleEmitterObject.cpp \
                    Particles/ParticleLayer.cpp \
                    Particles/ParticlePropertyLine.cpp \
                    Particles/ParticleSystem.cpp \
                    \
                    Platform/TemplateAndroid/CorePlatformAndroid.cpp \
                    \
                    Platform/Mutex.cpp \
                    Platform/SystemTimer.cpp \
                    Platform/Thread.cpp \
                    Platform/ThreadAndroid.cpp \
                    \
                    Render/2D/Font.cpp \
                    Render/2D/FontManager.cpp \
                    Render/2D/FTFont.cpp \
                    Render/2D/GraphicsFont.cpp \
                    Render/2D/Sprite.cpp \
                    Render/2D/TextBlock.cpp \
                    \
                    Render/3D/AnimatedMesh.cpp \
                    Render/3D/PolygonGroup.cpp \
                    Render/3D/StaticMesh.cpp \
                    \
                    Render/Effects/ColorOnlyEffect.cpp \
                    Render/Effects/MultiTextureEffect.cpp \
                    Render/Effects/TextureMulColorEffect.cpp \
                    \
                    Render/DynamicIndexBuffer.cpp \
                    Render/DynamicVertexBuffer.cpp \
                    Render/Image.cpp \
                    Render/LibPngHelpers.cpp \
                    Render/RenderBase.cpp \
                    Render/RenderDataObject.cpp \
                    Render/RenderEffect.cpp \
                    Render/RenderGrayscaleEffect.cpp \
                    Render/RenderHelper.cpp \
                    Render/RenderManager.cpp \
                    Render/RenderManagerGL.cpp \
                    Render/RenderManagerGL20.cpp \
                    Render/RenderManagerFactory.cpp \
                    Render/RenderResource.cpp \
                    Render/RenderStateBlock.cpp \
                    Render/Shader.cpp \
                    Render/ShaderGL.cpp \
                    Render/StaticIndexBuffer.cpp \
                    Render/StaticVertexBuffer.cpp \
                    Render/Texture.cpp \
                    \
                    Scene2D/Box2DDebugDraw.cpp \
                    Scene2D/Box2DGameObject.cpp \
                    Scene2D/Box2DGameObjectManager.cpp \
                    Scene2D/Box2DHelper.cpp \
                    Scene2D/Box2DTileCollider.cpp \
                    Scene2D/GameObject.cpp \
                    Scene2D/GameObjectAnimations.cpp \
                    Scene2D/GameObjectManager.cpp \
                    \
                    Scene3D/BoneNode.cpp \
                    Scene3D/Camera.cpp \
                    Scene3D/Frustum.cpp \
                    Scene3D/LandscapeNode.cpp \
                    Scene3D/MeshInstanceNode.cpp \
                    Scene3D/PathManip.cpp \
                    Scene3D/RotatingCubeNode.cpp \
                    Scene3D/Scene.cpp \
                    Scene3D/SceneAnimationMixer.cpp \
                    Scene3D/SceneFile.cpp \
                    Scene3D/SceneNode.cpp \
                    Scene3D/SceneNode3d.cpp \
                    Scene3D/SceneNodeAnimation.cpp \
                    Scene3D/SceneNodeAnimationKey.cpp \
                    Scene3D/SceneNodeAnimationList.cpp \
                    Scene3D/SkeletonNode.cpp \
                    Scene3D/SpriteNode.cpp \
                    \
                    Sound/Sound.cpp \
                    Sound/SoundBuffer.cpp \
                    Sound/SoundChannel.cpp \
                    Sound/SoundDataProvider.cpp \
                    Sound/SoundGroup.cpp \
                    Sound/SoundInstance.cpp \
                    Sound/SoundOVProvider.cpp \
                    Sound/SoundSystem.cpp \
                    Sound/SoundWVProvider.cpp \
                    \
                    UI/UI3DView.cpp \
                    UI/ScrollHelper.cpp \
                    UI/UIButton.cpp \
                    UI/UIControl.cpp \
                    UI/UIControlBackground.cpp \
                    UI/UIControlSystem.cpp \
                    UI/UIEvent.cpp \
                    UI/UIFadeTransition.cpp \
                    UI/UIFileSystemDialog.cpp \
                    UI/UIHierarchy.cpp \
                    UI/UIHierarchyCell.cpp \
                    UI/UIHierarchyNode.cpp \
                    UI/UIHoleTransition.cpp \
                    UI/UIJoypad.cpp \
                    UI/UIList.cpp \
                    UI/UIListCell.cpp \
                    UI/UILoadingTransition.cpp \
                    UI/UIMoveInTransition.cpp \
                    UI/UIPopup.cpp \
                    UI/UIScreen.cpp \
                    UI/UIScreenManagerAndroid.cpp \
                    UI/UIScreenTransition.cpp \
                    UI/UIScrollBar.cpp \
                    UI/UISlider.cpp \
                    UI/UIStaticText.cpp \
                    UI/UITextField.cpp \
                    UI/UIYamlLoader.cpp \
                    \
                    Utils/HTTPDownloaderAndroid.cpp \
                    Utils/MD5.cpp \
                    Utils/StringFormat.cpp \
                    Utils/UTF8Utils.cpp \
                    Utils/Utils.cpp \

# set build flags
LOCAL_CFLAGS := -frtti -g -O0

# set exported build flags
LOCAL_EXPORT_CFLAGS := $(LOCAL_CFLAGS)

# set used libs
LOCAL_LDLIBS := -lGLESv1_CM -llog -lGLESv2
LOCAL_LDLIBS += $(LOCAL_PATH)/../../Libs/libs/libzip_android.a
LOCAL_LDLIBS += $(LOCAL_PATH)/../../Libs/libs/libxml_android.a
LOCAL_LDLIBS += $(LOCAL_PATH)/../../Libs/libs/libpng_android.a
LOCAL_LDLIBS += $(LOCAL_PATH)/../../Libs/libs/libfreetype_android.a
LOCAL_LDLIBS += $(LOCAL_PATH)/../../Libs/libs/libyaml_android.a

# set exported used libs
LOCAL_EXPORT_LDLIBS := $(LOCAL_LDLIBS)

# set included libraries
LOCAL_STATIC_LIBRARIES := libbox2d

include $(BUILD_STATIC_LIBRARY)

# include modules
$(call import-module,box2d)
