from ViewerPythonBinding.lib import ViewerPythonBinding

viewer = ViewerPythonBinding.ViewerApp(
    "/home/shinaraka/Projects/OpenGLTemplate/data/sample_bunny.json")
viewer.launch()
del viewer

viewerGUI = ViewerPythonBinding.ViewerGUIApp()
viewerGUI.launch()
del viewerGUI
