export LD_LIBRARY_PATH=../../
JAVA=/usr/lib/java/jre/bin/java
JAVAC=/usr/lib/jvm/java-1.4.2-sun/bin/javac

if [ ! -f main.class ];then
	$JAVAC -classpath .:./SaX.jar main.java
fi
$JAVA -classpath .:./SaX.jar main
