/*
 * Small tool to convert a wireshark package dump to plain
 * hex representation. For a sample dump file look at research/sniffed_packages/.
 * NOTE The package header changes
 */

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;

public class TrimWiresharkPackageDump
{
  public static void main(String[] args) throws IOException
  {
    if(args.length != 1)
    {
      System.out.println("Usage: TrimWiresharkPackageDump [FILE]");
      return;
    }

    String path = args[0];
    File rawDataFile = new File(path);
    if(!rawDataFile.exists())
    {
      System.out.println("File does not exist.");
      return;
    }

    String data = new String(Files.readAllBytes(Paths.get(path)));
    data = data.replaceAll("\\d\\d\\d\\d  ", "");
    data = data.replaceAll("\\s{3}.*", "");

    //USB package data
    data = data.replaceAll("1b 00 00 ee 22 85 ff ff ff ff 00 00 00 00 09 00", "");
    data = data.replaceAll("00 01 00 01 00 01 01 40 00 00 00 ", "");

    data = data.replaceAll("(\\r\\n)|(\\n)", " ");
    data = data.replaceAll("  ", "");
    data = data.trim();
    data = data.replaceAll("aa48", "aa\n48");

    System.out.println(data);
  }
}
