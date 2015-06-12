/*
 Copyright 2010-2015 Tarik Sekmen.

 All Rights Reserved.

 Written by Tarik Sekmen <tarik@ilixi.org>.

 This file is part of ilixi.

 ilixi is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 ilixi is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with ilixi.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <graphics/Style.h>
#include <lib/FileSystem.h>
#include <core/PlatformManager.h>
#include <core/Logger.h>
#include <lib/XMLReader.h>
#include <libgen.h>
#include <fstream>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_STYLE, "ilixi/graphics/Style", "Style Parser");

Style::Style()
        : _pack(NULL)
{
}

Style::~Style()
{
    release();
}

void
Style::release()
{
    delete _pack;
}

bool
Style::parseStyle(const char* style)
{
    ILOG_TRACE(ILX_STYLE);
    ILOG_DEBUG(ILX_STYLE, " -> file: %s\n", style);

    std::string cacheFile = PrintF("%s%u.sxml", FileSystem::ilxDirectory().c_str(), createHash(style));
    ILOG_DEBUG(ILX_STYLE, " -> cache file: %s\n", cacheFile.c_str());
    if (difftime(FileSystem::getModificationTime(cacheFile), FileSystem::getModificationTime(style)) > 0)
    {
        ILOG_DEBUG(ILX_STYLE, " -> Parsing cached style file.\n");
        std::ifstream ifs(cacheFile.c_str(), std::ios::binary | std::ios::in);
        ifs >> *this;
        ifs.close();
        ILOG_INFO(ILX_STYLE, "Parsed cached style file: %s\n", cacheFile.c_str());
    } else
    {
        XMLReader xml;
        if (xml.loadFile(style) == false)
        {
            ILOG_FATAL(ILX_STYLE, "Could not parse style!\n");
            return false;
        }

        xmlNodePtr root = xml.root();
        xmlNodePtr group = xml.currentNode();

        release();



        xmlChar* imgFile = xmlGetProp(root, (xmlChar*) "resource");
        std::string path = (char*) imgFile;
        std::string file;
        size_t found = path.find("@IMGDIR:");
        if (found != std::string::npos)
        {
            file = ILIXI_DATADIR"images/";
            file.append(path.substr(found + 8, std::string::npos));
            ILOG_DEBUG(ILX_STYLE, " -> image file: %s\n", file.c_str());
        } else
        {
            found = path.find("@ILX_THEMEDIR:");
            if (found != std::string::npos)
            {
                file.append(PlatformManager::instance().getThemeDirectory());
                file.append(path.substr(found + 14, std::string::npos));
                ILOG_DEBUG(ILX_STYLE, " -> image file: %s\n", file.c_str());
            } else
            {
                ILOG_DEBUG(ILX_STYLE, " -> parsing theme...\n");
                char* path = strdup(style);
                path = dirname(path);
                std::string imgPack = std::string(std::string(path).append("/ui-pack.dfiff"));

                file = imgPack;
                ILOG_DEBUG(ILX_STYLE, " -> pack: %s\n", imgPack.c_str());
                free(path);
            }
        }

        _pack = new Image(file);
        xmlFree(imgFile);
        parseTheme(group);


        ILOG_INFO(ILX_STYLE, "Parsed style file: %s\n", style);

        // create cached file
        std::ofstream ofs(cacheFile.c_str(), std::ios::binary | std::ios::out);
        ofs.seekp(0, std::ios::beg);
        ofs << *this;
        ofs.close();
    }

    return true;
}

void
Style::parseTheme(xmlNodePtr node)
{
    xmlNodePtr state;
    while (node != NULL)
    {
        ILOG_DEBUG(ILX_STYLE, " Parsing %s...\n", node->name);
        if (xmlStrcmp(node->name, (xmlChar*) "PushButton") == 0)
        {
            state = node->children;
            while (state != NULL)
            {
                if (xmlStrcmp(state->name, (xmlChar*) "cell_3H") == 0)
                    getCell3H(state, pb.def);

                else if (xmlStrcmp(state->name, (xmlChar*) "pre") == 0)
                    getCell3HN(state, pb.def, pb.pre);

                else if (xmlStrcmp(state->name, (xmlChar*) "exp") == 0)
                    getCell3HN(state, pb.def, pb.exp);

                else if (xmlStrcmp(state->name, (xmlChar*) "dis") == 0)
                    getCell3HN(state, pb.def, pb.dis);

                else if (xmlStrcmp(state->name, (xmlChar*) "foc") == 0)
                    getCell3HN(state, pb.def, pb.foc);

                state = state->next;
            }
        } else if (xmlStrcmp(node->name, (xmlChar*) "PushButtonOK") == 0)
        {
            state = node->children;
            while (state != NULL)
            {
                if (xmlStrcmp(state->name, (xmlChar*) "cell_3H") == 0)
                    getCell3H(state, pbOK.def);

                else if (xmlStrcmp(state->name, (xmlChar*) "pre") == 0)
                    getCell3HN(state, pbOK.def, pbOK.pre);

                else if (xmlStrcmp(state->name, (xmlChar*) "exp") == 0)
                    getCell3HN(state, pbOK.def, pbOK.exp);

                else if (xmlStrcmp(state->name, (xmlChar*) "dis") == 0)
                    getCell3HN(state, pbOK.def, pbOK.dis);

                else if (xmlStrcmp(state->name, (xmlChar*) "foc") == 0)
                    getCell3HN(state, pbOK.def, pbOK.foc);

                state = state->next;
            }
        } else if (xmlStrcmp(node->name, (xmlChar*) "PushButtonCancel") == 0)
        {
            state = node->children;
            while (state != NULL)
            {
                if (xmlStrcmp(state->name, (xmlChar*) "cell_3H") == 0)
                    getCell3H(state, pbCAN.def);

                else if (xmlStrcmp(state->name, (xmlChar*) "pre") == 0)
                    getCell3HN(state, pbCAN.def, pbCAN.pre);

                else if (xmlStrcmp(state->name, (xmlChar*) "exp") == 0)
                    getCell3HN(state, pbCAN.def, pbCAN.exp);

                else if (xmlStrcmp(state->name, (xmlChar*) "dis") == 0)
                    getCell3HN(state, pbCAN.def, pbCAN.dis);

                else if (xmlStrcmp(state->name, (xmlChar*) "foc") == 0)
                    getCell3HN(state, pbCAN.def, pbCAN.foc);

                state = state->next;
            }
        } else if (xmlStrcmp(node->name, (xmlChar*) "ToolButton") == 0)
        {
            state = node->children;
            while (state != NULL)
            {
                if (xmlStrcmp(state->name, (xmlChar*) "cell_9") == 0)
                    getCell9(state, tb.def);

                else if (xmlStrcmp(state->name, (xmlChar*) "pre") == 0)
                    getCell9N(state, tb.def, tb.pre);

                else if (xmlStrcmp(state->name, (xmlChar*) "exp") == 0)
                    getCell9N(state, tb.def, tb.exp);

                else if (xmlStrcmp(state->name, (xmlChar*) "dis") == 0)
                    getCell9N(state, tb.def, tb.dis);

                else if (xmlStrcmp(state->name, (xmlChar*) "foc") == 0)
                    getCell9N(state, tb.def, tb.foc);

                state = state->next;
            }
        } else if (xmlStrcmp(node->name, (xmlChar*) "DirectionalButton1") == 0)
        {
            state = node->children;
            while (state != NULL)
            {
                if (xmlStrcmp(state->name, (xmlChar*) "cell_9") == 0)
                    getCell9(state, db1.def);

                else if (xmlStrcmp(state->name, (xmlChar*) "pre") == 0)
                    getCell9N(state, db1.def, db1.pre);

                else if (xmlStrcmp(state->name, (xmlChar*) "exp") == 0)
                    getCell9N(state, db1.def, db1.exp);

                else if (xmlStrcmp(state->name, (xmlChar*) "dis") == 0)
                    getCell9N(state, db1.def, db1.dis);

                else if (xmlStrcmp(state->name, (xmlChar*) "foc") == 0)
                    getCell9N(state, db1.def, db1.foc);

                state = state->next;
            }
        } else if (xmlStrcmp(node->name, (xmlChar*) "DirectionalButton2") == 0)
        {
            state = node->children;
            while (state != NULL)
            {
                if (xmlStrcmp(state->name, (xmlChar*) "cell_9") == 0)
                    getCell9(state, db2.def);

                else if (xmlStrcmp(state->name, (xmlChar*) "pre") == 0)
                    getCell9N(state, db2.def, db2.pre);

                else if (xmlStrcmp(state->name, (xmlChar*) "exp") == 0)
                    getCell9N(state, db2.def, db2.exp);

                else if (xmlStrcmp(state->name, (xmlChar*) "dis") == 0)
                    getCell9N(state, db2.def, db2.dis);

                else if (xmlStrcmp(state->name, (xmlChar*) "foc") == 0)
                    getCell9N(state, db2.def, db2.foc);

                state = state->next;
            }
        } else if (xmlStrcmp(node->name, (xmlChar*) "LineInput") == 0)
        {
            state = node->children;
            while (state != NULL)
            {
                if (xmlStrcmp(state->name, (xmlChar*) "cell_9") == 0)
                    getCell9(state, li.def);

                else if (xmlStrcmp(state->name, (xmlChar*) "dis") == 0)
                    getCell9N(state, li.def, li.dis);

                else if (xmlStrcmp(state->name, (xmlChar*) "foc") == 0)
                    getCell9N(state, li.def, li.foc);

                state = state->next;
            }
        } else if (xmlStrcmp(node->name, (xmlChar*) "LineInput2") == 0)
        {
            state = node->children;
            while (state != NULL)
            {
                if (xmlStrcmp(state->name, (xmlChar*) "cell_9") == 0)
                    getCell9(state, li2.def);

                else if (xmlStrcmp(state->name, (xmlChar*) "dis") == 0)
                    getCell9N(state, li2.def, li2.dis);

                else if (xmlStrcmp(state->name, (xmlChar*) "foc") == 0)
                    getCell9N(state, li2.def, li2.foc);

                state = state->next;
            }
        } else if (xmlStrcmp(node->name, (xmlChar*) "CheckBox") == 0)
        {
            state = node->children;
            while (state != NULL)
            {
                if (xmlStrcmp(state->name, (xmlChar*) "cell") == 0)
                    getCell(state, cb.def);

                else if (xmlStrcmp(state->name, (xmlChar*) "pre") == 0)
                    getCellN(state, cb.def, cb.pre);

                else if (xmlStrcmp(state->name, (xmlChar*) "exp") == 0)
                    getCellN(state, cb.def, cb.exp);

                else if (xmlStrcmp(state->name, (xmlChar*) "dis") == 0)
                    getCellN(state, cb.def, cb.dis);

                else if (xmlStrcmp(state->name, (xmlChar*) "foc") == 0)
                    getCellN(state, cb.def, cb.foc);

                state = state->next;
            }
        } else if (xmlStrcmp(node->name, (xmlChar*) "CheckBoxChecked") == 0)
        {
            state = node->children;
            while (state != NULL)
            {
                if (xmlStrcmp(state->name, (xmlChar*) "cell") == 0)
                    getCell(state, cbC.def);

                else if (xmlStrcmp(state->name, (xmlChar*) "pre") == 0)
                    getCellN(state, cbC.def, cbC.pre);

                else if (xmlStrcmp(state->name, (xmlChar*) "exp") == 0)
                    getCellN(state, cbC.def, cbC.exp);

                else if (xmlStrcmp(state->name, (xmlChar*) "dis") == 0)
                    getCellN(state, cbC.def, cbC.dis);

                else if (xmlStrcmp(state->name, (xmlChar*) "foc") == 0)
                    getCellN(state, cbC.def, cbC.foc);

                state = state->next;
            }
        } else if (xmlStrcmp(node->name, (xmlChar*) "CheckBoxTriChecked") == 0)
        {
            state = node->children;
            while (state != NULL)
            {
                if (xmlStrcmp(state->name, (xmlChar*) "cell") == 0)
                    getCell(state, cbT.def);

                else if (xmlStrcmp(state->name, (xmlChar*) "pre") == 0)
                    getCellN(state, cbT.def, cbT.pre);

                else if (xmlStrcmp(state->name, (xmlChar*) "exp") == 0)
                    getCellN(state, cbT.def, cbT.exp);

                else if (xmlStrcmp(state->name, (xmlChar*) "dis") == 0)
                    getCellN(state, cbT.def, cbT.dis);

                else if (xmlStrcmp(state->name, (xmlChar*) "foc") == 0)
                    getCellN(state, cbT.def, cbT.foc);

                state = state->next;
            }
        } else if (xmlStrcmp(node->name, (xmlChar*) "RadioButtonOn") == 0)
        {
            state = node->children;
            while (state != NULL)
            {
                if (xmlStrcmp(state->name, (xmlChar*) "cell") == 0)
                    getCell(state, rbOn.def);

                else if (xmlStrcmp(state->name, (xmlChar*) "pre") == 0)
                    getCellN(state, rbOn.def, rbOn.pre);

                else if (xmlStrcmp(state->name, (xmlChar*) "exp") == 0)
                    getCellN(state, rbOn.def, rbOn.exp);

                else if (xmlStrcmp(state->name, (xmlChar*) "dis") == 0)
                    getCellN(state, rbOn.def, rbOn.dis);

                else if (xmlStrcmp(state->name, (xmlChar*) "foc") == 0)
                    getCellN(state, rbOn.def, rbOn.foc);

                state = state->next;
            }
        } else if (xmlStrcmp(node->name, (xmlChar*) "RadioButtonOff") == 0)
        {
            state = node->children;
            while (state != NULL)
            {
                if (xmlStrcmp(state->name, (xmlChar*) "cell") == 0)
                    getCell(state, rbOff.def);

                else if (xmlStrcmp(state->name, (xmlChar*) "pre") == 0)
                    getCellN(state, rbOn.def, rbOff.pre);

                else if (xmlStrcmp(state->name, (xmlChar*) "exp") == 0)
                    getCellN(state, rbOn.def, rbOff.exp);

                else if (xmlStrcmp(state->name, (xmlChar*) "dis") == 0)
                    getCellN(state, rbOn.def, rbOff.dis);

                else if (xmlStrcmp(state->name, (xmlChar*) "foc") == 0)
                    getCellN(state, rbOn.def, rbOff.foc);

                state = state->next;
            }
        } else if (xmlStrcmp(node->name, (xmlChar*) "HProgressBar") == 0)
        {
            state = node->children;
            while (state != NULL)
            {
                if (xmlStrcmp(state->name, (xmlChar*) "cell_3H") == 0)
                    getCell3H(state, prH.def);

                else if (xmlStrcmp(state->name, (xmlChar*) "dis") == 0)
                    getCell3HN(state, prH.def, prH.dis);

                state = state->next;
            }
        } else if (xmlStrcmp(node->name, (xmlChar*) "HProgressBarFill") == 0)
        {
            state = node->children;
            while (state != NULL)
            {
                if (xmlStrcmp(state->name, (xmlChar*) "cell_3H") == 0)
                    getCell3H(state, prHI.def);

                else if (xmlStrcmp(state->name, (xmlChar*) "dis") == 0)
                    getCell3HN(state, prHI.def, prHI.dis);

                state = state->next;
            }
        } else if (xmlStrcmp(node->name, (xmlChar*) "VProgressBar") == 0)
        {
            state = node->children;
            while (state != NULL)
            {
                if (xmlStrcmp(state->name, (xmlChar*) "cell_3V") == 0)
                    getCell3V(state, prV.def);

                else if (xmlStrcmp(state->name, (xmlChar*) "dis") == 0)
                    getCell3VN(state, prV.def, prV.dis);

                state = state->next;
            }
        } else if (xmlStrcmp(node->name, (xmlChar*) "VProgressBarFill") == 0)
        {
            state = node->children;
            while (state != NULL)
            {
                if (xmlStrcmp(state->name, (xmlChar*) "cell_3V") == 0)
                    getCell3V(state, prVI.def);

                else if (xmlStrcmp(state->name, (xmlChar*) "dis") == 0)
                    getCell3VN(state, prVI.def, prVI.dis);

                state = state->next;
            }
        } else if (xmlStrcmp(node->name, (xmlChar*) "HSlider") == 0)
        {
            state = node->children;
            while (state != NULL)
            {
                if (xmlStrcmp(state->name, (xmlChar*) "cell_3H") == 0)
                    getCell3H(state, hSl.def);

                else if (xmlStrcmp(state->name, (xmlChar*) "dis") == 0)
                    getCell3HN(state, hSl.def, hSl.dis);

                else if (xmlStrcmp(state->name, (xmlChar*) "fill") == 0)
                    getCell3HN(state, hSl.def, hSl.fill);

                else if (xmlStrcmp(state->name, (xmlChar*) "fill_dis") == 0)
                    getCell3HN(state, hSl.def, hSl.fill_dis);

                state = state->next;
            }
        } else if (xmlStrcmp(node->name, (xmlChar*) "VSlider") == 0)
        {
            state = node->children;
            while (state != NULL)
            {
                if (xmlStrcmp(state->name, (xmlChar*) "cell_3V") == 0)
                    getCell3V(state, vSl.def);

                else if (xmlStrcmp(state->name, (xmlChar*) "dis") == 0)
                    getCell3VN(state, vSl.def, vSl.dis);

                else if (xmlStrcmp(state->name, (xmlChar*) "fill") == 0)
                    getCell3VN(state, vSl.def, vSl.fill);

                else if (xmlStrcmp(state->name, (xmlChar*) "fill_dis") == 0)
                    getCell3VN(state, vSl.def, vSl.fill_dis);

                state = state->next;
            }
        } else if (xmlStrcmp(node->name, (xmlChar*) "SliderIndicator") == 0)
        {
            state = node->children;
            while (state != NULL)
            {
                if (xmlStrcmp(state->name, (xmlChar*) "cell") == 0)
                    getCell(state, slI.def);

                else if (xmlStrcmp(state->name, (xmlChar*) "pre") == 0)
                    getCellN(state, slI.def, slI.pre);

                else if (xmlStrcmp(state->name, (xmlChar*) "exp") == 0)
                    getCellN(state, slI.def, slI.exp);

                else if (xmlStrcmp(state->name, (xmlChar*) "dis") == 0)
                    getCellN(state, slI.def, slI.dis);

                else if (xmlStrcmp(state->name, (xmlChar*) "foc") == 0)
                    getCellN(state, slI.def, slI.foc);

                state = state->next;
            }
        } else if (xmlStrcmp(node->name, (xmlChar*) "Frame") == 0)
        {
            state = node->children;
            while (state != NULL)
            {
                if (xmlStrcmp(state->name, (xmlChar*) "cell_9") == 0)
                    getCell9(state, fr.def);

                else if (xmlStrcmp(state->name, (xmlChar*) "dis") == 0)
                    getCell9N(state, fr.def, fr.dis);

                state = state->next;
            }
        } else if (xmlStrcmp(node->name, (xmlChar*) "Panel") == 0)
        {
            state = node->children;
            while (state != NULL)
            {
                if (xmlStrcmp(state->name, (xmlChar*) "cell_9") == 0)
                    getCell9(state, panel.def);

                else if (xmlStrcmp(state->name, (xmlChar*) "dis") == 0)
                    getCell9N(state, panel.def, panel.dis);

                else if (xmlStrcmp(state->name, (xmlChar*) "passive") == 0)
                    getCell9N(state, panel.def, panel.passive);

                state = state->next;
            }
        } else if (xmlStrcmp(node->name, (xmlChar*) "PanelInverse") == 0)
        {
            state = node->children;
            xmlChar* over_lap = xmlGetProp(node, (xmlChar*) "overlap");
            overlap = atoi((char*) over_lap);
            xmlFree(over_lap);

            while (state != NULL)
            {
                if (xmlStrcmp(state->name, (xmlChar*) "cell") == 0)
                    getCell(state, panelInv.tl);

                else if (xmlStrcmp(state->name, (xmlChar*) "topright") == 0)
                    getCellN(state, panelInv.tl, panelInv.tr);

                else if (xmlStrcmp(state->name, (xmlChar*) "botleft") == 0)
                    getCellN(state, panelInv.tl, panelInv.bl);

                else if (xmlStrcmp(state->name, (xmlChar*) "botright") == 0)
                    getCellN(state, panelInv.tl, panelInv.br);

                state = state->next;
            }
        } else if (xmlStrcmp(node->name, (xmlChar*) "PanelInverseDisabled") == 0)
        {
            state = node->children;
            while (state != NULL)
            {
                if (xmlStrcmp(state->name, (xmlChar*) "cell") == 0)
                    getCell(state, panelInvDis.tl);

                else if (xmlStrcmp(state->name, (xmlChar*) "topright") == 0)
                    getCellN(state, panelInvDis.tl, panelInvDis.tr);

                else if (xmlStrcmp(state->name, (xmlChar*) "botleft") == 0)
                    getCellN(state, panelInvDis.tl, panelInvDis.bl);

                else if (xmlStrcmp(state->name, (xmlChar*) "botright") == 0)
                    getCellN(state, panelInvDis.tl, panelInvDis.br);

                state = state->next;
            }
        } else if (xmlStrcmp(node->name, (xmlChar*) "HScroller") == 0)
        {
            getCell3H(node->children, hScr);
        } else if (xmlStrcmp(node->name, (xmlChar*) "VScroller") == 0)
        {
            getCell3V(node->children, vScr);
        } else if (xmlStrcmp(node->name, (xmlChar*) "ScrollBar") == 0)
        {
            getCell9(node->children, sb.def);
            getCell9N(node->children->next, sb.def, sb.dis);
        } else if (xmlStrcmp(node->name, (xmlChar*) "HScrollBarRidge") == 0)
        {
            getCell(node->children, sbRH);
        } else if (xmlStrcmp(node->name, (xmlChar*) "VScrollBarRidge") == 0)
        {
            getCell(node->children, sbRV);
        }

        else if (xmlStrcmp(node->name, (xmlChar*) "ToolBar") == 0)
        {
            getCell(node->children, tbar);
        } else if (xmlStrcmp(node->name, (xmlChar*) "ToolBarButton") == 0)
        {
            state = node->children;
            while (state != NULL)
            {
                if (xmlStrcmp(state->name, (xmlChar*) "cell_3H") == 0)
                    getCell3H(state, tbarb.def);

                else if (xmlStrcmp(state->name, (xmlChar*) "pre") == 0)
                    getCell3HN(state, tbarb.def, tbarb.pre);

                else if (xmlStrcmp(state->name, (xmlChar*) "exp") == 0)
                    getCell3HN(state, tbarb.def, tbarb.exp);

                else if (xmlStrcmp(state->name, (xmlChar*) "dis") == 0)
                    getCell3HN(state, tbarb.def, tbarb.dis);

                else if (xmlStrcmp(state->name, (xmlChar*) "foc") == 0)
                    getCell3HN(state, tbarb.def, tbarb.foc);

                state = state->next;
            }
        } else if (xmlStrcmp(node->name, (xmlChar*) "HLine") == 0)
        {
            getCell(node->children, hLine);
        } else if (xmlStrcmp(node->name, (xmlChar*) "VLine") == 0)
        {
            getCell(node->children, vLine);
        } else if (xmlStrcmp(node->name, (xmlChar*) "HToolButtonCheck") == 0)
        {
            state = node->children;
            while (state != NULL)
            {
                if (xmlStrcmp(state->name, (xmlChar*) "cell_3H") == 0)
                    getCell3H(state, tbIndH.def);

                else if (xmlStrcmp(state->name, (xmlChar*) "dis") == 0)
                    getCell3HN(state, tbIndH.def, tbIndH.dis);

                state = state->next;
            }
        } else if (xmlStrcmp(node->name, (xmlChar*) "VToolButtonCheck") == 0)
        {
            state = node->children;
            while (state != NULL)
            {
                if (xmlStrcmp(state->name, (xmlChar*) "cell_3V") == 0)
                    getCell3V(state, tbIndV.def);

                else if (xmlStrcmp(state->name, (xmlChar*) "dis") == 0)
                    getCell3VN(state, tbIndV.def, tbIndV.dis);

                state = state->next;
            }
        } else if (xmlStrcmp(node->name, (xmlChar*) "DialogPanel") == 0)
        {
            getCell9(node->children, dialog);
        }

        node = node->next;
    } // end while(element)
}

std::istream&
operator>>(std::istream& is, Style& obj)
{
    obj.release();

    obj._pack = new Image();
    is >> *obj._pack;

    is >> obj.pb.def;
    is >> obj.pb.pre;
    is >> obj.pb.exp;
    is >> obj.pb.dis;
    is >> obj.pb.foc;

    is >> obj.pbOK.def;
    is >> obj.pbOK.pre;
    is >> obj.pbOK.exp;
    is >> obj.pbOK.dis;
    is >> obj.pbOK.foc;

    is >> obj.pbCAN.def;
    is >> obj.pbCAN.pre;
    is >> obj.pbCAN.exp;
    is >> obj.pbCAN.dis;
    is >> obj.pbCAN.foc;

    is >> obj.tb.def;
    is >> obj.tb.pre;
    is >> obj.tb.exp;
    is >> obj.tb.dis;
    is >> obj.tb.foc;

    is >> obj.db1.def;
    is >> obj.db1.pre;
    is >> obj.db1.exp;
    is >> obj.db1.dis;
    is >> obj.db1.foc;

    is >> obj.db2.def;
    is >> obj.db2.pre;
    is >> obj.db2.exp;
    is >> obj.db2.dis;
    is >> obj.db2.foc;

    is >> obj.li.def;
    is >> obj.li.dis;
    is >> obj.li.foc;

    is >> obj.li2.def;
    is >> obj.li2.dis;
    is >> obj.li2.foc;

    is >> obj.cb.def;
    is >> obj.cb.pre;
    is >> obj.cb.exp;
    is >> obj.cb.dis;
    is >> obj.cb.foc;

    is >> obj.cbC.def;
    is >> obj.cbC.pre;
    is >> obj.cbC.exp;
    is >> obj.cbC.dis;
    is >> obj.cbC.foc;

    is >> obj.cbT.def;
    is >> obj.cbT.pre;
    is >> obj.cbT.exp;
    is >> obj.cbT.dis;
    is >> obj.cbT.foc;

    is >> obj.rbOn.def;
    is >> obj.rbOn.pre;
    is >> obj.rbOn.exp;
    is >> obj.rbOn.dis;
    is >> obj.rbOn.foc;

    is >> obj.rbOff.def;
    is >> obj.rbOff.pre;
    is >> obj.rbOff.exp;
    is >> obj.rbOff.dis;
    is >> obj.rbOff.foc;

    is >> obj.prH.def;
    is >> obj.prH.dis;

    is >> obj.prHI.def;
    is >> obj.prHI.dis;

    is >> obj.prV.def;
    is >> obj.prV.dis;

    is >> obj.prVI.def;
    is >> obj.prVI.dis;

    is >> obj.hSl.def;
    is >> obj.hSl.dis;
    is >> obj.hSl.fill;
    is >> obj.hSl.fill_dis;

    is >> obj.vSl.def;
    is >> obj.vSl.dis;
    is >> obj.vSl.fill;
    is >> obj.vSl.fill_dis;

    is >> obj.slI.def;
    is >> obj.slI.pre;
    is >> obj.slI.exp;
    is >> obj.slI.dis;
    is >> obj.slI.foc;

    is >> obj.fr.def;
    is >> obj.fr.dis;

    is >> obj.panel.def;
    is >> obj.panel.dis;
    is >> obj.panel.passive;

    is >> obj.overlap;
    is >> obj.panelInv.tl;
    is >> obj.panelInv.tr;
    is >> obj.panelInv.bl;
    is >> obj.panelInv.br;

    is >> obj.panelInvDis.tl;
    is >> obj.panelInvDis.tr;
    is >> obj.panelInvDis.bl;
    is >> obj.panelInvDis.br;

    is >> obj.hScr;
    is >> obj.vScr;

    is >> obj.sb.def;
    is >> obj.sb.dis;

    is >> obj.sbRH;
    is >> obj.sbRV;

    is >> obj.tbar;
    is >> obj.tbarb.def;
    is >> obj.tbarb.pre;
    is >> obj.tbarb.exp;
    is >> obj.tbarb.dis;
    is >> obj.tbarb.foc;

    is >> obj.hLine;
    is >> obj.vLine;

    is >> obj.tbIndH.def;
    is >> obj.tbIndH.dis;
    is >> obj.tbIndV.def;
    is >> obj.tbIndV.dis;

    is >> obj.dialog;

    return is;
}

std::ostream&
operator<<(std::ostream& os, const Style& obj)
{
    os << *obj._pack << std::endl;

    os << obj.pb.def;
    os << obj.pb.pre;
    os << obj.pb.exp;
    os << obj.pb.dis;
    os << obj.pb.foc;

    os << obj.pbOK.def;
    os << obj.pbOK.pre;
    os << obj.pbOK.exp;
    os << obj.pbOK.dis;
    os << obj.pbOK.foc;

    os << obj.pbCAN.def;
    os << obj.pbCAN.pre;
    os << obj.pbCAN.exp;
    os << obj.pbCAN.dis;
    os << obj.pbCAN.foc;

    os << obj.tb.def;
    os << obj.tb.pre;
    os << obj.tb.exp;
    os << obj.tb.dis;
    os << obj.tb.foc;

    os << obj.db1.def;
    os << obj.db1.pre;
    os << obj.db1.exp;
    os << obj.db1.dis;
    os << obj.db1.foc;

    os << obj.db2.def;
    os << obj.db2.pre;
    os << obj.db2.exp;
    os << obj.db2.dis;
    os << obj.db2.foc;

    os << obj.li.def;
    os << obj.li.dis;
    os << obj.li.foc;

    os << obj.li2.def;
    os << obj.li2.dis;
    os << obj.li2.foc;

    os << obj.cb.def << std::endl;
    os << obj.cb.pre << std::endl;
    os << obj.cb.exp << std::endl;
    os << obj.cb.dis << std::endl;
    os << obj.cb.foc << std::endl;

    os << obj.cbC.def << std::endl;
    os << obj.cbC.pre << std::endl;
    os << obj.cbC.exp << std::endl;
    os << obj.cbC.dis << std::endl;
    os << obj.cbC.foc << std::endl;

    os << obj.cbT.def << std::endl;
    os << obj.cbT.pre << std::endl;
    os << obj.cbT.exp << std::endl;
    os << obj.cbT.dis << std::endl;
    os << obj.cbT.foc << std::endl;

    os << obj.rbOn.def << std::endl;
    os << obj.rbOn.pre << std::endl;
    os << obj.rbOn.exp << std::endl;
    os << obj.rbOn.dis << std::endl;
    os << obj.rbOn.foc << std::endl;

    os << obj.rbOff.def << std::endl;
    os << obj.rbOff.pre << std::endl;
    os << obj.rbOff.exp << std::endl;
    os << obj.rbOff.dis << std::endl;
    os << obj.rbOff.foc << std::endl;

    os << obj.prH.def;
    os << obj.prH.dis;

    os << obj.prHI.def;
    os << obj.prHI.dis;

    os << obj.prV.def;
    os << obj.prV.dis;

    os << obj.prVI.def;
    os << obj.prVI.dis;

    os << obj.hSl.def;
    os << obj.hSl.dis;
    os << obj.hSl.fill;
    os << obj.hSl.fill_dis;

    os << obj.vSl.def;
    os << obj.vSl.dis;
    os << obj.vSl.fill;
    os << obj.vSl.fill_dis;

    os << obj.slI.def << std::endl;
    os << obj.slI.pre << std::endl;
    os << obj.slI.exp << std::endl;
    os << obj.slI.dis << std::endl;
    os << obj.slI.foc << std::endl;

    os << obj.fr.def;
    os << obj.fr.dis;

    os << obj.panel.def;
    os << obj.panel.dis;
    os << obj.panel.passive;

    os << obj.overlap << std::endl;
    os << obj.panelInv.tl << std::endl;
    os << obj.panelInv.tr << std::endl;
    os << obj.panelInv.bl << std::endl;
    os << obj.panelInv.br << std::endl;

    os << obj.panelInvDis.tl << std::endl;
    os << obj.panelInvDis.tr << std::endl;
    os << obj.panelInvDis.bl << std::endl;
    os << obj.panelInvDis.br << std::endl;

    os << obj.hScr;
    os << obj.vScr;

    os << obj.sb.def;
    os << obj.sb.dis;

    os << obj.sbRH << std::endl;
    os << obj.sbRV << std::endl;

    os << obj.tbar << std::endl;
    os << obj.tbarb.def << std::endl;
    os << obj.tbarb.pre << std::endl;
    os << obj.tbarb.exp << std::endl;
    os << obj.tbarb.dis << std::endl;
    os << obj.tbarb.foc << std::endl;

    os << obj.hLine << std::endl;
    os << obj.vLine << std::endl;

    os << obj.tbIndH.def << std::endl;
    os << obj.tbIndH.dis << std::endl;
    os << obj.tbIndV.def << std::endl;
    os << obj.tbIndV.dis << std::endl;

    os << obj.dialog << std::endl;

    return os;
}

} /* namespace ilixi */
