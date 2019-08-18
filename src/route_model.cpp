#include "route_model.h"
#include <iostream>

RouteModel::RouteModel(const std::vector<std::byte>& xml) : Model(xml)
{
    int counter = 0;
    for (Model::Node node : this->Nodes())
    {
        m_Nodes.push_back(Node(counter, this, node));
        counter++;
    }
    CreateNodeToRoadHashmap();
}

float RouteModel::Node::distance(RouteModel::Node obj_node) const
{

    return std::sqrt(std::pow((this->x - obj_node.x), 2.0f) + std::pow((this->y - obj_node.y), 2.0f));

}

void RouteModel::CreateNodeToRoadHashmap()
{
    for (const Model::Road& road : Roads())
    {
        if (road.type != Model::Road::Type::Footway)
        {
            for (int node_idx : Ways()[road.way].nodes)
            {

                if (node_to_road.find(node_idx) == node_to_road.end())
                {
                    node_to_road[node_idx] = std::vector<const Model::Road*>();
                }
                node_to_road[node_idx].push_back(&road);
            }
        }
    }
}

RouteModel::Node* RouteModel::Node::FindNeighbor(std::vector<int> node_indices)
{
    Node* closest_node = nullptr;
    Node node;

    for (int node_index : node_indices)
    {
        node = parent_model->SNodes()[node_index];
        if (this->distance(node) != 0 && !node.visited)
        {
            if (closest_node == nullptr || this->distance(node) < this->distance(*closest_node))
            {
                closest_node = &parent_model->SNodes()[node_index];
            }
        }
    }
    return closest_node;
}

void RouteModel::Node::FindNeighbors()
{
    for (auto& road : parent_model->node_to_road[this->index])
    {
        RouteModel::Node* new_neighbor = this->FindNeighbor(parent_model->Ways()[road->way].nodes);
        if (new_neighbor)
        {
            this->neighbors.emplace_back(new_neighbor);
        }
    }
}

RouteModel::Node& RouteModel::FindClosestNode(float x, float y)
{
    Node input;
    input.x = x;
    input.y = y;

    float min_distance = std::numeric_limits<float>::max();
    float dist;
    int closest_idx;

    for (const Model::Road& road : Roads())
    {
        if (road.type != Model::Road::Type::Footway)
        {
            for (int node_idx : Ways()[road.way].nodes)
            {
                dist = input.distance(SNodes()[node_idx]);
                if (dist < min_distance)
                {
                    closest_idx = node_idx;
                    min_distance = dist;
                }
            }
        }
    }
    return SNodes()[closest_idx];
}

