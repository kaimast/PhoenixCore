/*

Copyright (c) 2010, Jonathan Wayne Parrott

Please see the license.txt file included with this source
distribution for more information.

*/

#include "BMFontLoader.h"

using namespace phoenix;
using namespace std;
using namespace boost;

/* Loads a FNT file */
void BMFontLoader::load( const std::string& fnt_file ){
	fstream fh;
	string line;

	fh.open(fnt_file, fstream::in);

	if( !fh ) return;

	//find the directory (For loading pages)
	std::string directory = fnt_file;
	for( size_t n = 0; (n = directory.find('/', n)) != string::npos; ) directory.replace(n, 1, "\\");
	size_t i = directory.rfind('\\');
	if( i != string::npos )
		directory = directory.substr(0, i+1);
	else
		directory = "";

	/* Parse! */
	while( fh.good() ){
		getline( fh, line );
		
		//tokenize, parse.
		typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
		boost::char_separator<char> sep(" =\"");
		tokenizer tokens(line, sep);

		for (tokenizer::iterator it = tokens.begin(); it != tokens.end(); ++it) {
			string token = *it;

			/* Load Page Texture */
			if( token == "page" ){

				//parse the info.
				for (++it; it != tokens.end(); ++it) {
					string name = *it; 
					if( name == "file" ) {
						//load the texture, assign it.
						TexturePtr t = system.loadTexture(directory + (*++it));
						font->setTexture(t);
					}
				}

				break;
			}

			/* Load character data */
			if( token == "char" ){
				int id = 0;
				BitmapFont::Character c;

				for (++it; it != tokens.end(); ++it) {
					string name = *it; 
					if( name == "id" ){
						id = atoi( (*++it).c_str() );
					}
					else if( name == "x" ){
						c.x = atoi( (*++it).c_str() );
					}
					else if( name == "y" ){
						c.y = atoi( (*++it).c_str() );
					}
					else if( name == "width" ){
						c.width = atoi( (*++it).c_str() );
					}
					else if( name == "height" ){
						c.height = atoi( (*++it).c_str() );
					}
					else if( name == "xoffset" ){
						c.xoffset = atoi( (*++it).c_str() );
					}
					else if( name == "yoffset" ){
						c.yoffset = atoi( (*++it).c_str() );
					}
					else if( name == "xadvance" ){
						c.xadvance = atoi( (*++it).c_str() );
					}
				}
				
				if( id != 0 ){
					font->setCharacterProperties(id,c);
				}
				
				break;
			}

		}
	}

	fh.close();
}