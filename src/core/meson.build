source_file = [
    meson.current_source_dir().join('common/common.h'),
    meson.current_source_dir().join('common/logger.cpp'),
    meson.current_source_dir().join('common/logger.h'),
    meson.current_source_dir().join('common/singleInstance.h'),
    meson.current_source_dir().join('common/timer.h'),
    meson.current_source_dir().join('common/vec.h'),

    meson.current_source_dir().join('input/input.h'),
    meson.current_source_dir().join('input/inputAction.h'),
    meson.current_source_dir().join('input/inputController.h'),

    meson.current_source_dir().join('canvas/canvas.cpp'),
    meson.current_source_dir().join('canvas/canvas.h'),
    meson.current_source_dir().join('canvas/animationCreatorCanvas.cpp'),
    meson.current_source_dir().join('canvas/animationCreatorCanvas.h'),
    meson.current_source_dir().join('canvas/animationCreatorInputController.h'),
    meson.current_source_dir().join('canvas/animationCreatorInputController.cpp'),

    meson.current_source_dir().join('canvas/editMode/editMode.h'),
    meson.current_source_dir().join('canvas/editMode/pickMode.h'),
    meson.current_source_dir().join('canvas/editMode/pickMode.cpp'),
    meson.current_source_dir().join('canvas/editMode/addMode.cpp'),
    meson.current_source_dir().join('canvas/editMode/addMode.h'),
    meson.current_source_dir().join('canvas/editMode/addPathMode.cpp'),
    meson.current_source_dir().join('canvas/editMode/addPathMode.h'),

    meson.current_source_dir().join('canvas/shapeUtil.cpp'),
    meson.current_source_dir().join('canvas/shapeUtil.h'),

    meson.current_source_dir().join('gpu/gl/extraGl.cpp'),
    meson.current_source_dir().join('gpu/gl/extraGl.h'),
    meson.current_source_dir().join('gpu/gl/glUtil.cpp'),
    meson.current_source_dir().join('gpu/gl/glUtil.h'),

    meson.current_source_dir().join('scene/scene.h'),
    meson.current_source_dir().join('scene/scene.cpp'),
    meson.current_source_dir().join('scene/entity.h'),
    meson.current_source_dir().join('scene/entity.cpp'),
    meson.current_source_dir().join('scene/ui/bbox.cpp'),
    meson.current_source_dir().join('scene/ui/bbox.h'),
    meson.current_source_dir().join('scene/ui/controlBox.h'),
    meson.current_source_dir().join('scene/ui/controlBox.cpp'),
    meson.current_source_dir().join('scene/ui/editPath.cpp'),
    meson.current_source_dir().join('scene/ui/editPath.h'),

    meson.current_source_dir().join('scene/component/components.h'),
    meson.current_source_dir().join('scene/component/path.cpp'),
    meson.current_source_dir().join('scene/component/path.h'),
    meson.current_source_dir().join('scene/component/keyframe.h'),

    meson.current_source_dir().join('interface/editHelper.cpp'),
    meson.current_source_dir().join('interface/editHelper.h'),
    meson.current_source_dir().join('interface/editInterface.h'),
    meson.current_source_dir().join('interface/editInterface.cpp'),

    meson.current_source_dir().join('animation/animator.cpp'),
    meson.current_source_dir().join('animation/animator.h'),

    meson.current_source_dir().join('selection/selectionManager.h'),
    meson.current_source_dir().join('selection/selectionManager.cpp'),
]

core_dep += [declare_dependency(
    include_directories : include_directories('.'),
    sources             : source_file,
)]
