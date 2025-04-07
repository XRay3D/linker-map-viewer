#pragma once
#include <QDebug>

//import java.awt.event.ActionEvent;
//import java.awt.event.ActionListener;
import java.io.File;
#include <vector>

//import javax.swing.JFileChooser;
//import javax.swing.JMenu;
//import javax.swing.JMenuBar;
//import javax.swing.JMenuItem;
//import javax.swing.filechooser.FileNameExtensionFilter;

class MemoryViewerMenu:public JMenuBar implements ActionListener
{
    

	// Attributes
    private: JMenu menuFile;
    private: JMenuItem menuItemLoad;
    private: MemoryInfoFactory memoryInfoFactory;
    private: std::vector<MemoryInfoUpdateListener> memoryInfoUpdateListeners;
	
	// Constructor
    public: MemoryViewerMenu () {
		
		// Initialize attributes
		memoryInfoFactory = new MemoryInfoFactory();
        memoryInfoUpdateListeners = new std::vector<MemoryInfoUpdateListener>();
		
		// Create contents
		menuFile = new JMenu(u"File"_s);
		menuItemLoad = new JMenuItem(u"Load"_s);
		
		// Add listeners
		menuItemLoad.addActionListener(this);
		
		// Bind menu
		add(menuFile);
		menuFile.add(menuItemLoad);
	}

	// Loader
    public: void load (File file)
	{
		// Load the map, configuration and contents
		memoryInfoFactory.load(file);
		
		// Inform listeners of the update
		for (int i = 0; i < memoryInfoUpdateListeners.size(); i++) {
			memoryInfoUpdateListeners.get(i).update(memoryInfoFactory);
		}
	}
	
	// Menu action listener
    public: void actionPerformed (ActionEvent e)
	{
		if (e.getSource() == menuItemLoad) {
			
			// Load a file
			FileNameExtensionFilter extensionFilter = new FileNameExtensionFilter(u"map files (*.map)"_s, u"map"_s);
			JFileChooser fileChooser = new JFileChooser();
			fileChooser.setFileFilter(extensionFilter);
			if (fileChooser.showOpenDialog(this) == JFileChooser.APPROVE_OPTION) {
				load(fileChooser.getSelectedFile());
			}
		}
	}
	
	// Other methods
    public: void addMemoryInfoUpdateListener (MemoryInfoUpdateListener memoryInfoUpdateListener) {
		memoryInfoUpdateListeners.add(memoryInfoUpdateListener);
	}
};
