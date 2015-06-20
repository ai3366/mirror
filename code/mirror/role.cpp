#include "role.h"
#include <QMessageBox>
#include <QFile>
#include <QMouseEvent>
#include "dlg_count.h"
#include "role_skill.h"
#include "def_takInfo.h"
#include "task.h"

extern QWidget *g_widget;

extern QVector<Info_Item> g_ItemList;
extern QVector<Info_basic_equip> g_EquipList;
extern QMap<itemID, Info_StateEquip> g_StateEquip;
extern QVector<Info_jobAdd> g_JobAddSet;
extern roleAddition g_roleAddition;
extern QVector<info_task> g_task_main_list;
Dlg_Detail *m_dlg_detail;

role::role(RoleInfo *roleInfo, VecRoleSkill *skill, MapItem *bag_item, MapItem *storage_item, ListEquip *bag_equip, ListEquip *storage_equip)
: QWidget(NULL)
, myRole(roleInfo)
, m_skill_study(skill)
, m_bag_item(bag_item)
, m_storage_item(storage_item)
, m_bag_equip(bag_equip)
, m_storage_equip(storage_equip)
, m_tab_itemBag(bag_item,roleInfo)
, m_tab_equipBag(roleInfo, bag_equip, storage_equip)
, m_tab_equipStorage(roleInfo, bag_equip, storage_equip)
{
	ui.setupUi(this);

	ui.edit_test_1->setVisible(false);
	ui.edit_test_2->setVisible(false);
	ui.edit_test_3->setVisible(false);
	ui.btn_test->setVisible(false);
	ui.checkBox_autoSave->setVisible(false);
	// 将控件保存到窗口中，方便后续直接采用循环处理
	EquipmentGrid.append(ui.lbl_equip_0);
	EquipmentGrid.append(ui.lbl_equip_1);
	EquipmentGrid.append(ui.lbl_equip_3);
	EquipmentGrid.append(ui.lbl_equip_4);
	EquipmentGrid.append(ui.lbl_equip_51);
	EquipmentGrid.append(ui.lbl_equip_52);
	EquipmentGrid.append(ui.lbl_equip_61);
	EquipmentGrid.append(ui.lbl_equip_62);
	EquipmentGrid.append(ui.lbl_equip_7);
	EquipmentGrid.append(ui.lbl_equip_8);
	EquipmentGrid.append(ui.lbl_equip_9);
	EquipmentGrid.append(ui.lbl_equip_10);

	EquipPos[0] = ui.lbl_equip_0->pos();
	EquipPos[1] = ui.lbl_equip_1->pos();
	EquipPos[2] = ui.lbl_equip_3->pos();
	bShifePress = false;

	m_dlg_detail = new Dlg_Detail(this);
	m_dlg_detail->setWindowFlags(Qt::WindowStaysOnTopHint);

	myRole->lvExp = g_JobAddSet[myRole->level].exp;
	if (myRole->gender == 1)
	{
		ui.lbl_role_backimg->setPixmap(QPixmap(":/ui/Resources/ui/1.png"));
	}
	else
	{
		ui.lbl_role_backimg->setPixmap(QPixmap(":/ui/Resources/ui/2.png"));
	}

 	ui.tabWidget_bag->addTab(&m_tab_equipBag, QStringLiteral("装备"));
 	ui.tabWidget_bag->addTab(&m_tab_itemBag, QStringLiteral("道具"));
 	ui.tabWidget_bag->addTab(&m_tab_equipStorage, QStringLiteral("装备仓库"));
// 	ui.tabWidget_bag->addTab(&m_tab_storageItem, QStringLiteral("道具仓库"));

	DisplayEquip();
	DisplayRoleInfo();
	m_tab_itemBag.updateInfo();
	m_tab_equipBag.updateInfo();
	m_tab_equipStorage.updateInfo();

	//为装备栏控件安装事件过滤机制，使得QLabel控件可响应clicked()之类的事件。
	foreach (QLabel *lbl, EquipmentGrid)
	{
		lbl->installEventFilter(this);
	}

	QObject::connect(&m_tab_equipBag, &Item_Base::UpdateEquipInfoSignals, this, &role::UpdateEquipInfo);
	QObject::connect(&m_tab_equipStorage, &Item_Base::UpdateEquipInfoSignals, this, &role::UpdateEquipInfo);
	QObject::connect(&m_tab_equipBag, &item_equipBag::wearEquip, this, &role::on_wearEquip);
	QObject::connect(&m_tab_equipBag, &Item_Base::UpdatePlayerInfoSignals, this, &role::updateRoleInfo);
	QObject::connect(&m_tab_itemBag, &item_itemBag::UsedItem, this, &role::on_usedItem);
	QObject::connect(&m_tab_itemBag, &Item_Base::UpdatePlayerInfoSignals, this, &role::updateRoleInfo);
}

role::~role()
{

}

void role::updateRoleInfo(void)
{
 	DisplayRoleInfo();
 	
 	m_tab_itemBag.updateInfo();
 	m_tab_equipBag.updateInfo();
}

void role::UpdateEquipInfo(void)
{
 	m_tab_equipBag.updateInfo();
 	m_tab_equipStorage.updateInfo();
}
void role::UpdateItemInfo(void)
{
	m_tab_itemBag.updateInfo();
}

void role::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Shift)
	{
		bShifePress = true;
	}
}
void role::keyReleaseEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Shift)
	{
		bShifePress = false;
	}
}
inline void role::Broken32Bit(quint32 nSrc, quint8 &n1, quint8 &n2, quint8 &n3, quint8 &n4)
{
	n1 = nSrc >> 24;
	n2 = (nSrc >> 16) & 0xFF;
	n3 = (nSrc >> 8) & 0xFF;
	n4 = nSrc & 0xFF;
}

void role::DisplayRoleInfo(void)
{
	QString strTmp;
	qint32 nTmp;
	quint32 nTmp1, nTmp2;

	ui.edit_role_name->setText(myRole->name);
	ui.edit_role_vocation->setText(def_vocation[myRole->vocation]);
	ui.edit_role_coin->setText(QString::number(myRole->coin));
	ui.edit_role_reputation->setText(QString::number(myRole->reputation));
	ui.edit_role_level->setText(QString::number(myRole->level));

	ui.edit_role_strength->setText(QString::number(g_roleAddition.strength));
	ui.edit_role_wisdom->setText(QString::number(g_roleAddition.wisdom));
	ui.edit_role_spirit->setText(QString::number(g_roleAddition.spirit));
	ui.edit_role_life->setText(QString::number(g_roleAddition.life));
	ui.edit_role_agility->setText(QString::number(g_roleAddition.agility));
	ui.edit_role_potential->setText(QString::number(g_roleAddition.potential));

	strTmp = QString::number(myRole->exp) + "/" + QString::number(myRole->lvExp);
	ui.edit_role_exp->setText(strTmp);

	nTmp = qMax(quint32(1000), 1500 - g_roleAddition.agility);
	myRole->intervel_1 = (nTmp >> 8) & 0xff;
	myRole->intervel_2 = nTmp & 0xff;
	ui.edit_role_interval->setText(QString::number(nTmp));

	const Info_jobAdd &jobAdd = g_JobAddSet[myRole->level - 1];

	nTmp1 = jobAdd.dc1 + equip_add.dc1 + g_roleAddition.strength / 10;
	nTmp2 = jobAdd.dc2 + equip_add.dc2 + g_roleAddition.strength / 5;
	if (nTmp2 < nTmp1)
	{
		nTmp2 = nTmp1;			//确保上限 >= 下限
	}
	ui.edit_role_dc->setText(QString("%1-%2").arg(nTmp1).arg(nTmp2));
	Broken32Bit(nTmp1, myRole->dc1_1, myRole->dc1_2, myRole->dc1_3, myRole->dc1_4);
	Broken32Bit(nTmp2, myRole->dc2_1, myRole->dc2_2, myRole->dc2_3, myRole->dc2_4);

	nTmp1 = jobAdd.mc1 + equip_add.mc1 + g_roleAddition.wisdom / 10;
	nTmp2 = jobAdd.mc2 + equip_add.mc2 + g_roleAddition.wisdom / 5;
	if (nTmp2 < nTmp1)
	{
		nTmp2 = nTmp1;
	}
	ui.edit_role_mc->setText(QString("%1-%2").arg(nTmp1).arg(nTmp2));
	Broken32Bit(nTmp1, myRole->mc1_1, myRole->mc1_2, myRole->mc1_3, myRole->mc1_4);
	Broken32Bit(nTmp2, myRole->mc2_1, myRole->mc2_2, myRole->mc2_3, myRole->mc2_4);

	nTmp1 = jobAdd.sc1 + equip_add.sc1 + g_roleAddition.spirit / 10;
	nTmp2 = jobAdd.sc2 + equip_add.sc2 + g_roleAddition.spirit / 5;
	if (nTmp2 < nTmp1)
	{
		nTmp2 = nTmp1;
	}
	ui.edit_role_sc->setText(QString("%1-%2").arg(nTmp1).arg(nTmp2));
	Broken32Bit(nTmp1, myRole->sc1_1, myRole->sc1_2, myRole->sc1_3, myRole->sc1_4);
	Broken32Bit(nTmp2, myRole->sc2_1, myRole->sc2_2, myRole->sc2_3, myRole->sc2_4);

	nTmp1 = jobAdd.ac1 + equip_add.ac1 + g_roleAddition.strength / 13;
	nTmp2 = jobAdd.ac2 + equip_add.ac2 + g_roleAddition.strength / 7;
	if (nTmp2 < nTmp1)
	{
		nTmp2 = nTmp1;
	}
	ui.edit_role_ac->setText(QString("%1-%2").arg(nTmp1).arg(nTmp2));
	Broken32Bit(nTmp1, myRole->ac1_1, myRole->ac1_2, myRole->ac1_3, myRole->ac1_4);
	Broken32Bit(nTmp2, myRole->ac2_1, myRole->ac2_2, myRole->ac2_3, myRole->ac2_4);

	nTmp1 = jobAdd.mac1 + equip_add.mac1 + g_roleAddition.wisdom / 15 + g_roleAddition.spirit / 14;
	nTmp2 = jobAdd.mac2 + equip_add.mac2 + g_roleAddition.wisdom / 8 + g_roleAddition.spirit / 7;
	if (nTmp2 < nTmp1)
	{
		nTmp2 = nTmp1;
	}
	ui.edit_role_mac->setText(QString("%1-%2").arg(nTmp1).arg(nTmp2));
	Broken32Bit(nTmp1, myRole->mac1_1, myRole->mac1_2, myRole->mac1_3, myRole->mac1_4);
	Broken32Bit(nTmp2, myRole->mac2_1, myRole->mac2_2, myRole->mac2_3, myRole->mac2_4);

	myRole->luck = equip_add.luck & 0xFF;
	ui.edit_role_luck->setText(QString::number(myRole->luck));

	nTmp1 = jobAdd.hp + g_roleAddition.life * 25;
	ui.edit_role_hp->setText(QString::number(nTmp1));
	Broken32Bit(nTmp1, myRole->hp_1, myRole->hp_2, myRole->hp_3, myRole->hp_4);

	nTmp1 = jobAdd.mp + g_roleAddition.life * 15;
	ui.edit_role_mp->setText(QString::number(nTmp1));
	Broken32Bit(nTmp1, myRole->mp_1, myRole->mp_2, myRole->mp_3, myRole->mp_4);

	if (g_roleAddition.potential <= 0)
	{
		ui.btn_role_strength->setDisabled(true);
		ui.btn_role_wisdom->setDisabled(true);
		ui.btn_role_spirit->setDisabled(true);
		ui.btn_role_life->setDisabled(true);
		ui.btn_role_agility->setDisabled(true);
	}
	else
	{
		ui.btn_role_strength->setDisabled(false);
		ui.btn_role_wisdom->setDisabled(false);
		ui.btn_role_spirit->setDisabled(false);
		ui.btn_role_life->setDisabled(false);
		ui.btn_role_agility->setDisabled(false);
	}

	if (myRole->level % 100 == 99 || myRole->level >= MaxLv || myRole->exp < myRole->lvExp)
	{
		ui.btn_role_lvUp->setDisabled(true);
	}
	else
	{
		ui.btn_role_lvUp->setDisabled(false);
	}
}
void role::EquipAddPara_Add(const Info_basic_equip &equip, const EquipExtra &extra, quint32 lvUp)
{
	equip_add.acc += equip.acc + extra.acc;
	equip_add.luck += equip.luck + extra.luck; 
	equip_add.ac1 += equip.ac1;
	equip_add.ac2 += equip.ac2 + extra.ac;
	equip_add.mac1 += equip.mac1;
	equip_add.mac2 += equip.mac2 + extra.mac;
	equip_add.dc1 += equip.dc1;
	equip_add.dc2 += equip.dc2 + extra.dc;
	equip_add.mc1 += equip.mc1;
	equip_add.mc2 += equip.mc2 + extra.mc;
	equip_add.sc1 += equip.sc1;
	equip_add.sc2 += equip.sc2 + extra.sc;
}
void role::EquipAddPara_Sub(const Info_basic_equip &equip, const EquipExtra &extra, quint32 lvUp)
{
	equip_add.acc -= equip.acc + extra.acc;
	equip_add.luck -= equip.luck + extra.luck;
	equip_add.ac1 -= equip.ac1;
	equip_add.ac2 -= equip.ac2 + extra.ac;
	equip_add.mac1 -= equip.mac1;
	equip_add.mac2 -= equip.mac2 + extra.mac;
	equip_add.dc1 -= equip.dc1;
	equip_add.dc2 -= equip.dc2 + extra.dc;
	equip_add.mc1 -= equip.mc1;
	equip_add.mc2 -= equip.mc2 + extra.mc;
	equip_add.sc1 -= equip.sc1;
	equip_add.sc2 -= equip.sc2 + extra.sc;
}
void role::DisplayEquip()
{
	memset(&equip_add, 0, sizeof(Info_basic_equip));

	for (quint32 i = 0; i < MaxEquipCountForRole; i++)
	{
		if (g_roleAddition.vecEquip[i].ID == 0)
		{
			continue;				//当前部位无装备
		}

		for (QVector<Info_basic_equip>::const_iterator iter = g_EquipList.begin(); iter != g_EquipList.end(); iter++)
		{
			if (g_roleAddition.vecEquip[i].ID == iter->ID)
			{
				EquipmentGrid[i]->setPixmap(iter->icon); 
				EquipAddPara_Add(*iter, g_roleAddition.vecEquip[i].extra, g_roleAddition.vecEquip[i].lvUp);
				break;
			}
		}
	}

	if (g_roleAddition.vecEquip[0].ID != 0)
	{
		const Info_StateEquip &stateEquip = g_StateEquip[g_roleAddition.vecEquip[0].ID];
		EquipmentGrid[0]->setPixmap(stateEquip.img);
		EquipmentGrid[0]->resize(stateEquip.img.size());
		EquipmentGrid[0]->move((EquipPos[0])-(QPoint(stateEquip.offset_x, stateEquip.offset_y)));
	}
	if (g_roleAddition.vecEquip[1].ID != 0)
	{
		EquipmentGrid[1]->setPixmap(g_StateEquip[g_roleAddition.vecEquip[1].ID].img);
	}
	if (g_roleAddition.vecEquip[2].ID != 0)
	{
		const Info_StateEquip &stateEquip = g_StateEquip[g_roleAddition.vecEquip[2].ID];
		EquipmentGrid[2]->setPixmap(stateEquip.img);
		EquipmentGrid[2]->resize(stateEquip.img.size());
		EquipmentGrid[2]->move((EquipPos[2]) - (QPoint(stateEquip.offset_x, stateEquip.offset_y)));
	}
}

void role::on_btn_mirror_save_clicked()
{
	emit mirrorSave();
}
void role::on_btn_role_strength_clicked()
{
	int n = 1;
	if (bShifePress) {
		if (g_roleAddition.potential < 10)	{
			n = g_roleAddition.potential;
		}	else	{
			n = 10;
		}
	}
	g_roleAddition.potential -= n;
	g_roleAddition.strength += n;
	DisplayRoleInfo();
}
void role::on_btn_role_wisdom_clicked()
{
	int n = 1;
	if (bShifePress) {
		if (g_roleAddition.potential < 10)	{
			n = g_roleAddition.potential;
		}	else	{
			n = 10;
		}
	}
	g_roleAddition.potential -= n;
	g_roleAddition.wisdom += n;
	DisplayRoleInfo();
}
void role::on_btn_role_spirit_clicked()
{
	int n = 1;
	if (bShifePress) {
		if (g_roleAddition.potential < 10)	{
			n = g_roleAddition.potential;
		}	else	{
			n = 10;
		}
	}
	g_roleAddition.potential -= n;
	g_roleAddition.spirit += n;
	DisplayRoleInfo();
}
void role::on_btn_role_life_clicked()
{
	int n = 1;
	if (bShifePress) {
		if (g_roleAddition.potential < 10)	{
			n = g_roleAddition.potential;
		}	else	{
			n = 10;
		}
	}
	g_roleAddition.potential -= n;
	g_roleAddition.life += n;
	DisplayRoleInfo();
}
void role::on_btn_role_agility_clicked()
{
	int n = 1;
	if (bShifePress) {
		if (g_roleAddition.potential < 10)	{
			n = g_roleAddition.potential;
		}	else	{
			n = 10;
		}
	}
	g_roleAddition.potential -= n;
	g_roleAddition.agility += n;
	DisplayRoleInfo();
}
void role::on_btn_role_lvUp_clicked()
{
	myRole->exp -= myRole->lvExp;
	++myRole->level;
	myRole->lvExp = g_JobAddSet[myRole->level].exp;
	g_roleAddition.potential += 5;
	DisplayRoleInfo();
}
void role::on_wearEquip(quint32 ID_for_new, quint32 index)
{
	const Info_Equip &equip_new = m_bag_equip->at(index);
	const Info_basic_equip *EquipBasicInfo_new = Item_Base::GetEquipBasicInfo(equip_new.ID);
	
	//获取待佩带装备的类别
	int Type = (equip_new.ID - g_itemID_start_equip) / 1000;

	//根据类别映射到穿戴部位
	qint32 locationA, locationB;	
	locationA = locationB = -1;
	switch (Type)
	{
	case 1: locationA = 0; break;
	case 2: locationA = 1; break;
	case 3: locationA = 1; break;
	case 4: locationA = 2; break;
	case 5: locationA = 3; break;
	case 6: locationA = 4; locationB = 5; break;
	case 7: locationA = 6; locationB = 7; break;
	case 8: locationA = 8; break;
	case 9: locationA = 9; break;
	case 10: locationA = 10; break;
	case 11: locationA = 11; break;
	default:
		break;
	}

	//此装备可选装备左手/右手
	if (locationB != -1)
	{	//若左手有装备，右手为空，则装备在右手。否则装备在左手
		if (g_roleAddition.vecEquip[locationA].ID != 0 && g_roleAddition.vecEquip[locationB].ID == 0)
		{
			locationA = locationB;
		}
	}

	//扣除装备属性加成；将装备放入背包。
	const Info_basic_equip *EquipBasicInfo_old = Item_Base::GetEquipBasicInfo(g_roleAddition.vecEquip[locationA].ID);
	if (EquipBasicInfo_old != nullptr)
	{
		EquipAddPara_Sub(*EquipBasicInfo_old, g_roleAddition.vecEquip[locationA].extra, g_roleAddition.vecEquip[locationA].lvUp);
		m_bag_equip->append(g_roleAddition.vecEquip[locationA]);
	}
	//将背包装备从背包中取出来（删除）；更新装备属性加成，并显示相关信息
	EquipAddPara_Add(*EquipBasicInfo_new, equip_new.extra, equip_new.lvUp);
	g_roleAddition.vecEquip[locationA] = equip_new;
	m_bag_equip->removeAt(index);

	//更新特戒信息
	if (EquipBasicInfo_old != nullptr && EquipBasicInfo_old->ID == 307019)
	{
		quint32 skillId = 220037;
		for (qint32 i = 0; i < m_skill_study->size(); i++)
		{
			if (m_skill_study->at(i).id == skillId)
			{
				m_skill_study->removeAt(i);
				break;
			}
		}
		for (qint32 i = 0; i < myRole->skill.size(); i++)
		{
			if (myRole->skill.at(i).id == skillId)
			{
				myRole->skill.removeAt(i);
				break;
			}
		}
	}
	if (EquipBasicInfo_new->ID == 307019)
	{
		//火焰戒指
		roleSkill skill = {220037, 1};
		bool bWasStudy = false;
		for (qint32 i = 0; i < m_skill_study->size(); i++)
		{
			if (m_skill_study->at(i).id == skill.id)
			{
				bWasStudy = true;
			}
		}
		if (!bWasStudy)
		{
			m_skill_study->append(skill);
		}
	}
	
	if (locationA == 0 || locationA == 1 || locationA == 2)
	{
		const Info_StateEquip &stateEquip = g_StateEquip[g_roleAddition.vecEquip[locationA].ID];
		EquipmentGrid[locationA]->setPixmap(stateEquip.img);
		EquipmentGrid[locationA]->resize(stateEquip.img.size());
		EquipmentGrid[locationA]->move((EquipPos[locationA]) - (QPoint(stateEquip.offset_x, stateEquip.offset_y)));
	}
	else
	{
		EquipmentGrid[locationA]->setPixmap(EquipBasicInfo_new->icon);
	}
	updateRoleInfo();
	m_tab_equipBag.updateInfo();
}
void role::on_btn_test_clicked()
{
	qint32 locationA = ui.edit_test_1->text().toInt();
	qint32 posX = ui.edit_test_2->text().toInt();
	qint32 posY = ui.edit_test_3->text().toInt();
	EquipmentGrid[locationA]->move((EquipPos[locationA]) - (QPoint(posX, posY)));

}
void role::on_usedItem(quint32 ID)
{
	quint32 ItemCount = m_bag_item->value(ID);
	quint32 usedCount, nTmp;
	roleSkill skill;
	QString strTmp;

	//弹出对话框询问使用数量。
	dlg_count *dlg = new dlg_count(this, QStringLiteral("使用量"), ItemCount);
	if (QDialog::Accepted != dlg->exec())
		return;

	usedCount = dlg->getCount();
	if (usedCount <= 0)
		return;
	delete dlg;
	
	ItemCount -= usedCount;
	if (ItemCount <= 0)
		m_bag_item->remove(ID);
	else
		m_bag_item->insert(ID, ItemCount);
	m_tab_itemBag.updateInfo();

	//加成对应效果
	const Info_Item * itemItem = Item_Base::FindItem_Item(ID);
	nTmp = itemItem->value * usedCount;
	switch (itemItem->type)
	{
	case et_immediate_coin:		
		myRole->coin += nTmp;
		ui.edit_role_coin->setText(QString::number(myRole->coin));
		strTmp = QStringLiteral("金币增加：") + QString::number(nTmp);
		break;
	case et_immediate_gold:
		myRole->gold += nTmp;
		strTmp = QStringLiteral("元宝增加：") + QString::number(nTmp);
		break;
	case et_immediate_reputation:
		myRole->reputation += nTmp;
		ui.edit_role_reputation->setText(QString::number(myRole->reputation));
		strTmp = QStringLiteral("声望增加：") + QString::number(nTmp);
		break;
	case et_skill:
		skill.id = itemItem->ID;
		skill.level = (usedCount > 3 ? 3 : usedCount);
		for (VecRoleSkill::iterator iter = m_skill_study->begin(); iter != m_skill_study->end(); iter++)
		{
			if (iter->id == skill.id)
			{
				//已学会此技能，技能等级加1
				iter->level += usedCount;
				if (iter->level > 3)
				{
					iter->level = 3;
				}
				return;
			}
		}
		//新学此技能。
		m_skill_study->append(skill);
		strTmp = itemItem->descr;
		return;
	case et_ResetPotential:
		//重置角色属属点.
		nTmp = g_roleAddition.strength + g_roleAddition.wisdom + g_roleAddition.spirit + g_roleAddition.life + g_roleAddition.agility + g_roleAddition.potential;
		g_roleAddition.strength = g_roleAddition.wisdom = g_roleAddition.spirit = g_roleAddition.life = g_roleAddition.agility = 0;
		g_roleAddition.potential = nTmp;
		DisplayRoleInfo();
		strTmp = QStringLiteral("因为感悟混沌的力量，你的属性点已重置！");
		break;
	default:
		break;
	}

	if (!strTmp.isEmpty())
	{
		QMessageBox *msgBox = new QMessageBox;
		msgBox->setText(strTmp);
		msgBox->setWindowTitle(QStringLiteral("提示"));
		msgBox->addButton(QStringLiteral(" 我知道了 "), QMessageBox::AcceptRole);
		msgBox->exec();
	}
}
void role::on_btn_skill_clicked()
{
	role_skill *dlg_skill = new role_skill(this, m_skill_study, &myRole->skill);
	dlg_skill->setWindowFlags(Qt::Tool);
	//dlg_skill->move((pos()));
	dlg_skill->show();
}
void role::on_btn_task_clicked()
{
	task *taskDlg = new task(this);
	taskDlg->exec();
	delete taskDlg;
}
void role::DisplayEquipInfo(QPoint pos, const Info_basic_equip *BasicInfo, const Info_Equip *Equip)
{
	m_dlg_detail->DisplayEquipInfo(pos + QPoint(10, 10), BasicInfo, Equip, myRole);
	m_dlg_detail->show();
}

bool role::eventFilter(QObject *obj, QEvent *ev)
{
	if (ev->type() == QEvent::MouseButtonRelease)
	{
		QMouseEvent *mouseEvent = (QMouseEvent *)(ev);
		if (mouseEvent->button() == Qt::MouseButton::LeftButton)
		{	//左键显示装备详细信息
			for (quint32 i = 0; i < EquipmentGrid.size(); i++)
			{
				if (obj == EquipmentGrid[i])
				{
					const Info_Equip &equip = g_roleAddition.vecEquip[i];
					if (equip.ID != 0)
					{				
						const Info_basic_equip *EquipBasicInfo = Item_Base::GetEquipBasicInfo(equip.ID);
						if (EquipBasicInfo != nullptr)
						{
							DisplayEquipInfo(this->mapFromGlobal(mouseEvent->globalPos()), EquipBasicInfo, &equip);
							return  true;
						}
					}
				}
			}
		}
		else if (mouseEvent->button() == Qt::MouseButton::RightButton)
		{
			//右键取下装备
			for (quint32 i = 0; i < EquipmentGrid.size(); i++)
			{
				if (obj == EquipmentGrid[i])
				{
					const Info_Equip &equip = g_roleAddition.vecEquip[i];
					if (equip.ID != 0)
					{
						const Info_basic_equip *EquipBasicInfo_old = Item_Base::GetEquipBasicInfo(equip.ID);
						if (EquipBasicInfo_old != nullptr)
						{
							EquipAddPara_Sub(*EquipBasicInfo_old, equip.extra, equip.lvUp);
							m_bag_equip->append(equip);
							g_roleAddition.vecEquip[i] = { 0 };

							m_tab_equipBag.updateInfo();
							EquipmentGrid[i]->setPixmap(QPixmap(""));
							updateRoleInfo();

							if (EquipBasicInfo_old->ID == 307019)
							{
								quint32 skillId = 220037;
								for (qint32 i = 0; i < m_skill_study->size(); i++)
								{
									if (m_skill_study->at(i).id == skillId)
									{
										m_skill_study->removeAt(i);
										break;
									}
								}
								for (qint32 i = 0; i < myRole->skill.size(); i++)
								{
									if (myRole->skill.at(i).id == skillId)
									{
										myRole->skill.removeAt(i);
										break;
									}
								}
							}
							return  true;
						}
					}
				}
			}
		}
	}
	// pass the event on to the parent class
	return QWidget::eventFilter(obj, ev);		
}