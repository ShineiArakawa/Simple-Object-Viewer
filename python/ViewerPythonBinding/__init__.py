from .lib import ViewerPythonBinding


def launch_viewer_window(config_file_path: str):
    return ViewerPythonBinding.launchViewerWindow(config_file_path)
