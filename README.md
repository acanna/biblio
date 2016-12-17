# **biblio**
This tool helps to find bibliographic information for your pdf files. It generates .bib and .html files for youк single file or directory.

**USAGE:**

./main  {-p | \<files\> ... | -d \<path\> ... } [-c \<filename\>] [-b] [-f] [--] [--version] [-h]
 - \<files\>      	*(accepted multiple times)* **(OR required)**  *file names*
 - -d \<path\>,  --directory \<path\>     	*(accepted multiple times)* **(OR required)**  *directories for recursive search of PDF documents*
 - -p,  --purge     	**(OR required)** *purges database from non-existent files.*
 - -c <filename>,  --config <filename>   	 *name of the config file*
 - -b,  --database   	   *disable database*
 - -f,  --offline  	   *does only offline part*
 - --,  --ignore_rest   	  *ignores the rest of the labeled arguments following this flag*
 - --version    	*displays version information and exits*
 - -h,  --help  	  *displays usage information and exits*


**Dependencies:**
 - curl
 - poppler-cpp
 - tesseract 
 - leptonica 
 - config++ 
 - opencv_core, opencv_highgui, opencv_imgproc
 - tinyxml2 
 - sqlite3 
 - tclap 
 - jsoncpp
         
To fill config file it is necessary to register at [Elsevier] (http://dev.elsevier.com/) (for access to Scopus and ScienceDirect) and [Springer] (https://dev.springer.com/).
Obtained API-keys insert into biblio.cfg.
