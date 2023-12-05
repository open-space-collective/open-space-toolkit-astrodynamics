import xml.etree.ElementTree as ET


def remove_elements(xml_file, kinds_to_remove):
    """
    Parse the given XML file and remove elements with specified kinds.

    :param xml_file: Path to the XML file to be processed.
    :param kinds_to_remove: List of kinds to be removed from the XML.
    """
    # Parse the XML file
    tree = ET.parse(xml_file)
    root = tree.getroot()

    # Iterate over all elements and find those with the specified kinds
    for element in root.findall(".//compound"):
        if element.get("kind") in kinds_to_remove:
            root.remove(element)

    # Write the modified tree back to the file
    tree.write(xml_file, encoding="utf-8", xml_declaration=True)


# Example usage
xml_file_path = "xml/index.xml"  # Replace with your XML file path
kinds_to_remove = ["file", "define", "namespace", "dir"]
remove_elements(xml_file_path, kinds_to_remove)
